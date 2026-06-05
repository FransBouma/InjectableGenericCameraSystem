--[[
================================================================================
  RTTI Explorer v5 - Interactive Cheat Engine Script
  Based on: https://framedsc.com/GeneralGuides/using_rtti.htm
  (Information originally distilled from Hatti's video)

  Automates the full RTTI discovery pipeline:
    1. Scan for all .?AV type descriptors in the target module
    2. Browse/filter/export discovered type names
    3. For a selected type, walk the RTTI chain:
       Type Descriptor -> Object Locator -> VTable -> Live Instances
    4. Browse instance memory directly from the results

  v2 additions:
    5. Reverse lookup: given an instance pointer, walk backward through
       RTTI to recover the type name + full class hierarchy
       Instance -> VTable -> Object Locator -> Type Descriptor -> Name

  v3 additions:
    6. VTable Function Explorer: list all virtual function pointers
       from an instance's vtable
    7. Disassembly Viewer: click a vtable function to see full
       disassembly in CE memory viewer format

  v4 additions:
    8. COL.offset awareness for multiple-inheritance support. Each
       RTTICompleteObjectLocator carries a DWORD `offset` field at +0x04
       which is the byte offset of its vtable within the COMPLETE object:
         offset == 0 -> primary vtable (start of complete object)
         offset >  0 -> secondary vtable (a multi-inherited base subobject)
       v4 captures this for every vtable and:
         - Marks primary / secondary in the instance list.
         - Computes the COMPLETE OBJECT address for secondary subobject hits
           (complete = subobject_addr - COL.offset), so double-clicking,
           dissecting, hex-dumping and "Add to CE Table" land on the full
           object rather than on a slice of it.
         - Reverse lookup ("Identify Type") reports both the subobject
           address you pasted in and the inferred complete object address.

  v5 additions:
    9. Relaxed COL validation, matching etra's RTTI dumper algorithm. v4
       required each candidate to pass BOTH a signature check AND a
       pClassHierarchy-in-module check before being accepted as a COL.
       The pClassHier check turned out to be too strict in roughly 20% of
       real-world cases on x64 -- typically due to CE reporting an
       incomplete module size, non-contiguous module images, or RTTI
       structures relocated by runtime tooling -- silently rejecting real
       COLs and their vtables.

       v5 drops the pClassHier in-module requirement and instead validates
       COLs the way etra does: by the simple fact that something in the
       module's .rdata actually references them as a pointer. If a
       candidate at addr-0xC has the right signature byte AND at least one
       vtable backref exists, it's a real COL. If it has no backrefs, it
       was a coincidence and we drop it silently.

       This catches the previously-missed 20%, costs one extra scan per
       signature-passing candidate (cheap), and produces no false
       positives because no spurious "COL" can manifest a real vtable ref.

  Architecture support:
    Forward and reverse walks both branch on targetIs64Bit():
      x64 - TypeDescriptor name at +0x10, Object Locator uses RVAs,
            signature = 1, pointer/scan width = QWORD.
      x86 - TypeDescriptor name at +0x08, Object Locator stores absolute
            pointers (no pSelf field), signature = 0, pointer/scan
            width = DWORD. All 32-bit pointer reads go through readDword()
            so high addresses don't come back signed-negative from Lua.

  Tested against CE 7.4+ Lua API.
  
    Additional (Manual changes) made by Otis_Inf: Fixed vibecoded splitterbars
	Added range identifying features
================================================================================
--]]

-- ============================================================================
-- GLOBALS
-- ============================================================================
local SCRIPT_TITLE         = "RTTI Explorer v5"
local rttiTypes            = {}   -- { {address=, name=}, ... }
local filteredTypes        = {}
local foundInstances       = {}
local mainForm             = nil
local targetModule         = nil  -- selected module name
local moduleCache          = nil  -- cached module list for reverse lookups
local vtableFunctions      = {}   -- { {index=, address=, name=}, ... }
local selectedInstanceAddr = nil  -- tracks currently shown vtable
local moduleBaseAddress    = nil
local moduleSize 		   = nil
local moduleEnd			   = nil

-- Scrollbar constants (fallback if CE doesn't define them as globals)
if ssBoth    == nil then ssBoth    = 3 end
if ssVertical == nil then ssVertical = 2 end

-- ============================================================================
-- UTILITY
-- ============================================================================

local function log(msg)
	if mainForm and mainForm.memoLog then
		mainForm.memoLog.Lines.Add(os.date("[%H:%M:%S] ") .. tostring(msg))
		-- scroll to bottom
		mainForm.memoLog.SelStart = 999999999
		mainForm.memoLog.SelLength = 0
		processMessages()
	end
	print(msg)
end

local function hexStr(val)
	if val == nil then return "nil" end
	return string.format("0x%X", val)
end

--- Read an unsigned 32-bit value. CE's readInteger may return a signed value,
--- which would corrupt any pointer arithmetic when the high bit is set.
local function readDword(addr)
	local v = readInteger(addr)
	if v == nil then return nil end
	if v < 0 then v = v + 0x100000000 end
	return v
end

--- Read a pointer-sized value (QWORD on x64, unsigned DWORD on x86)
local function readPtr(addr)
	if targetIs64Bit() then
		return readQword(addr)
	else
		return readDword(addr)
	end
end

--- Read the SizeOfImage from the PE header to get true module extent
local function getModuleSizeFromPE(baseAddress)
	local peOffset = readInteger(baseAddress + 0x3C)
	if peOffset == nil or peOffset == 0 then return nil end
	-- OptionalHeader starts at PE + 0x18; SizeOfImage is at +0x38 for both PE32/PE32+
	local optHdrStart = baseAddress + peOffset + 0x18
	local sizeOfImage = readInteger(optHdrStart + 0x38)
	return sizeOfImage
end

--- Build module list using CE's enumModules()
local function getModuleList()
	local result = {}
	local proc = getOpenedProcessID()
	if not proc or proc == 0 then return result end

	local mods = enumModules()
	if mods == nil then return result end

	-- enumModules() may return a Lua table (1-indexed) or an object with .Count (0-indexed)
	if type(mods) == "table" then
		-- Plain Lua table (CE 7.5+)
		for i, entry in ipairs(mods) do
			local name = entry.Name or entry.name or tostring(i)
			local base = entry.Address or entry.address or 0
			local size = entry.Size or entry.size
			if (size == nil or size == 0) and base > 0 then
				size = getModuleSizeFromPE(base)
			end
			table.insert(result, {name = name, base = base, size = size or 0})
		end
	else
		-- Object with .Count (older CE)
		for i = 0, mods.Count - 1 do
			local entry = mods[i]
			local name = entry.Name
			local base = entry.Address
			local size = entry.Size
			if (size == nil or size == 0) and base > 0 then
				size = getModuleSizeFromPE(base)
			end
			table.insert(result, {name = name, base = base, size = size or 0})
		end
	end
	return result
end

local function getModuleInfo(modName)
	local mods = getModuleList()
	for _, m in ipairs(mods) do
		if m.name:lower() == modName:lower() then
			return m
		end
	end
	return nil
end

--- Refresh the cached module list (for reverse lookups)
local function refreshModuleCache()
	moduleCache = getModuleList()
end

--- Find which loaded module contains a given address
local function findModuleForAddress(addr)
	if not moduleCache then
		refreshModuleCache()
	end
	for _, m in ipairs(moduleCache) do
		if m.size > 0 and addr >= m.base and addr < (m.base + m.size) then
			return m
		end
	end
	return nil
end

-- ============================================================================
-- DEMANGLING
-- ============================================================================

--- Basic MSVC RTTI name demangling
--- .?AVClassName@Namespace@@ -> class Namespace::ClassName
--- .?AUStructName@@          -> struct StructName
--- .?AW4EnumName@@           -> enum EnumName
local function demangleMSVC(name)
	if not name or #name < 5 then return name end

	local prefix = ""
	local inner = name

	if name:sub(1, 4) == ".?AV" then
		prefix = "class "
		inner = name:sub(5)
	elseif name:sub(1, 4) == ".?AU" then
		prefix = "struct "
		inner = name:sub(5)
	elseif name:sub(1, 5) == ".?AW4" then
		prefix = "enum "
		inner = name:sub(6)
	else
		return name
	end

	-- Strip trailing @@
	inner = inner:gsub("@@$", "")

	-- Split on @ and reverse (MSVC stores innermost-first)
	local parts = {}
	for part in inner:gmatch("[^@]+") do
		table.insert(parts, 1, part)
	end

	return prefix .. table.concat(parts, "::")
end

-- ============================================================================
-- CORE RTTI LOGIC (FORWARD)
-- ============================================================================

--- Helper: perform a memory scan for a numeric value (hex) in a range
--- opts (optional table):
---   protection  - CE protection flag string, e.g. "+W-X" (default "" = all memory)
---   alignType   - fsmAligned / fsmNotAligned / fsmLastDigits (default fsmNotAligned)
---   alignParam  - alignment value as string (default "1")
--- Returns table of numeric addresses
local function scanForHexValue(value, valueType, startAddr, stopAddr, label, opts)
	opts = opts or {}
	local protection = opts.protection or ""
	local alignType  = opts.alignType  or fsmNotAligned
	local alignParam = opts.alignParam or "1"

	local ms = createMemScan()
	local fl = createFoundList(ms)

	-- firstScan parameter order:
	--  1  scanOption
	--  2  varType
	--  3  roundingType
	--  4  input1 (value string)
	--  5  input2 (nil for exact)
	--  6  startAddress
	--  7  stopAddress
	--  8  protectionFlags (string, "" = all)
	--  9  alignmentType
	-- 10  alignmentParam (string)
	-- 11  isHexadecimalInput   *** MUST be true for hex strings ***
	-- 12  isNotABinaryString
	-- 13  isUnicode
	-- 14  isCaseSensitive
	ms.firstScan(soExactValue, valueType, rtRounded,
		string.format("%X", value), nil,
		startAddr, stopAddr,
		protection, alignType, alignParam,
		true,   -- hex input
		true,   -- not binary string
		false,  -- not unicode
		false)  -- case irrelevant

	ms.waitTillDone()
	fl.initialize()

	local results = {}
	local count = fl.getCount()
	log("  [" .. (label or "Scan") .. "] " .. count .. " result(s)")

	for i = 0, count - 1 do
		local addrStr = fl.getAddress(i)
		table.insert(results, getAddress(addrStr))
	end

	fl.destroy()
	ms.destroy()
	return results
end

local function scanForRTTINameMarker(rttiTypes, modInfo, modEnd, nameMarkerString)
	local ms = createMemScan()
	local fl = createFoundList(ms)

	ms.firstScan(soExactValue, vtString, rtRounded,
		nameMarkerString, nil,
		modInfo.base, modEnd,
		"", fsmNotAligned, "1",
		false,  -- not hex (literal string search)
		true,   -- not a binary string
		false,  -- not unicode
		true)   -- case sensitive

	ms.waitTillDone()
	fl.initialize()

	local count = fl.getCount()
	log(string.format("Raw matches for name marker '%s': %d", nameMarkerString, count))

	if count < 10 then
		log("WARNING: Very few RTTI entries. This binary may lack usable RTTI.")
	end

	for i = 0, count - 1 do
		local addrStr = fl.getAddress(i)
		local addrNum = getAddress(addrStr)
		local val = readString(addrNum, 200, false)
		if val and #val > 0 then
			table.insert(rttiTypes, {address = addrNum, name = val})
		end
		-- yield every 500 entries so the UI doesn't freeze
		if i % 500 == 0 then processMessages() end
	end

	fl.destroy()
	ms.destroy()
end


--- Phase 1: Scan the target module for all ".?AV" RTTI type descriptor strings
local function scanForRTTITypes(modName)
	rttiTypes = {}
	filteredTypes = {}

	local modInfo = getModuleInfo(modName)
	if not modInfo then
		log("ERROR: Could not find module '" .. modName .. "'")
		return false
	end
	if modInfo.size == 0 then
		log("ERROR: Could not determine module size for '" .. modName .. "'")
		return false
	end

	local modEnd = modInfo.base + modInfo.size
	log("Scanning: " .. modInfo.name ..
		" | Base: " .. hexStr(modInfo.base) ..
		" | End: "  .. hexStr(modEnd) ..
		" | Size: " .. hexStr(modInfo.size))

	moduleEnd = modEnd
	moduleBaseAddress = modInfo.base
	moduleSize = modInfo.size

	scanForRTTINameMarker(rttiTypes, modInfo, modEnd, ".?AV")		-- classes
	scanForRTTINameMarker(rttiTypes, modInfo, modEnd, ".?AU")		-- struct

	table.sort(rttiTypes, function(a, b) return a.name < b.name end)
	filteredTypes = rttiTypes

	log("Indexed " .. #rttiTypes .. " RTTI type names.")
	return true
end

--- Phase 2: For a given type name, walk to the Object Locator
local function findObjectLocator(typeName, modName)
	local modInfo = getModuleInfo(modName)
	if not modInfo then
		log("ERROR: Module not found: " .. modName)
		return nil
	end

	local is64        = targetIs64Bit()
	local ptrSize     = is64 and 8 or 4
	local ptrType     = is64 and vtQword or vtDword
	local nameOffset  = is64 and 0x10 or 0x08
	local expectedSig = is64 and 1 or 0

	log("--- Walking RTTI chain for: " .. typeName .. " ---")
	log("Architecture: " .. (is64 and "x64 (RVA-based)" or "x86 (direct pointers)"))

	-- Find the address of the type name string from our earlier scan
	local typeStringAddr = nil
	for _, t in ipairs(rttiTypes) do
		if t.name == typeName then
			typeStringAddr = t.address
			break
		end
	end
	if not typeStringAddr then
		log("ERROR: Type name not in scan results.")
		return nil
	end
	log("Name string at: " .. hexStr(typeStringAddr))

	-- TypeDescriptor layout:
	--   x64:  +0x00 pVFTable (8)  +0x08 spare (8)  +0x10 name[]
	--   x86:  +0x00 pVFTable (4)  +0x04 spare (4)  +0x08 name[]
	local typeDescAddr = typeStringAddr - nameOffset
	log("TypeDescriptor at: " .. hexStr(typeDescAddr))

	-- RTTICompleteObjectLocator:
	--   +0x00  signature         (DWORD; x64=1, x86=0)
	--   +0x04  offset            (DWORD)
	--   +0x08  cdOffset          (DWORD)
	--   +0x0C  pTypeDescriptor   (DWORD RVA on x64 / DWORD absolute ptr on x86) <-- we scan for this
	--   +0x10  pClassHierarchy   (DWORD RVA on x64 / DWORD absolute ptr on x86)
	--   +0x14  pSelf             (DWORD RVA, x64 only)
	local searchValue
	if is64 then
		searchValue = typeDescAddr - modInfo.base
		log("TypeDescriptor RVA: " .. hexStr(searchValue))
		log("Scanning for TypeDesc RVA as DWORD in module...")
	else
		searchValue = typeDescAddr
		log("Scanning for TypeDesc absolute pointer as DWORD in module...")
	end

	local modEnd = modInfo.base + modInfo.size
	local candidates = scanForHexValue(searchValue, vtDword, modInfo.base, modEnd, "TypeDesc refs")

	if #candidates == 0 then
		log("ERROR: No references to TypeDescriptor found.")
		return nil
	end

	-- Each TypeDesc reference might be:
	--   (a) at offset +0x0C of an RTTICompleteObjectLocator  (what we want)
	--   (b) at offset +0x00 of an RTTIBaseClassDescriptor    (unrelated for our purposes)
	--
	-- We don't try to definitively distinguish (a) from (b) up front. We just
	-- compute (candidate - 0xC) for every reference and apply a single cheap
	-- filter: the DWORD at that address must equal the COL signature byte
	-- (0 on x86, 1 on x64). This rejects nearly all BCD refs (whose 12-bytes-
	-- before-the-BCD slot is essentially random) at minimal cost. We do NOT
	-- check pClassHierarchy here -- that check turned out to be too strict
	-- in real-world cases (CE under-reporting module size, non-contiguous
	-- images, runtime RTTI relocation).
	--
	-- The REAL validation happens in Phase 3: a true COL must have at least
	-- one vtable in the module that stores its address. If a signature-passing
	-- candidate has zero vtable backrefs, it was a coincidence and gets
	-- dropped silently. If it has backrefs, it's a real COL by definition --
	-- this is the same validation strategy etra's rtti_dumper uses.
	--
	-- For each candidate we also capture its `offset` field at COL+0x04 --
	-- the byte offset of THIS vtable within the complete object. offset==0
	-- marks the primary vtable; offset>0 means this COL belongs to a
	-- secondary multi-inherited base subobject, and a complete object
	-- pointer is recovered as (subobj_addr - offset).
	local modBase = modInfo.base
	local modEnd  = modInfo.base + modInfo.size
	local sigPassingCOLs = {}

	log("Considering " .. #candidates .. " TypeDesc reference(s)...")

	for _, addr in ipairs(candidates) do
		local candidateAddr = addr - 0xC
		local sig = readInteger(candidateAddr)

		if sig == expectedSig then
			local colOffset = readDword(candidateAddr + 0x04) or 0
			local role = (colOffset == 0) and "primary"
				or ("secondary +0x" .. string.format("%X", colOffset))
			table.insert(sigPassingCOLs, { addr = candidateAddr, offset = colOffset })
			log("  " .. hexStr(addr) .. " -> COL " .. hexStr(candidateAddr) ..
				"  sig=" .. hexStr(sig) ..
				"  offset=" .. hexStr(colOffset) .. " (" .. role .. ")" ..
				"  (pending vtable-ref validation)")
		end
		-- Candidates whose signature mismatches are silently skipped -- they're
		-- almost always RTTIBaseClassDescriptor refs or random data.
	end

	if #sigPassingCOLs == 0 then
		log("ERROR: No candidate had the expected COL signature byte.")
		log("  TypeDesc had " .. #candidates .. " reference(s); none looked like a COL.")
		log("  Most likely: this class has no virtual functions, is an abstract")
		log("  template instantiation, or its COL was stripped by the linker.")
		log("  RTTI is only emitted for classes that are actually polymorphic.")
		return nil
	end

	-- Pre-sort: primary COLs first, then by offset ascending. Just for nicer
	-- ordering of the eventual instance list -- primary hits come out first.
	table.sort(sigPassingCOLs, function(a, b) return a.offset < b.offset end)

	log(#sigPassingCOLs .. " candidate COL(s) passed signature check; " ..
		"vtable-ref existence will be the final filter.")

	return {
		typeDescAddr      = typeDescAddr,
		objectLocatorAddr = sigPassingCOLs[1].addr,  -- back-compat (first COL addr)
		objectLocators    = sigPassingCOLs,          -- list of { addr, offset }
		moduleBase        = modInfo.base,
		moduleSize        = modInfo.size,
		is64              = is64,
		ptrSize           = ptrSize,
		ptrType           = ptrType
	}
end

--- Phase 3: From Object Locator(s), find VTable(s), then scan all memory for instances
local function findVTableAndInstances(rttiInfo)
	local modBase    = rttiInfo.moduleBase
	local modSize    = rttiInfo.moduleSize
	local modEnd     = modBase + modSize
	local is64       = rttiInfo.is64
	local ptrSize    = rttiInfo.ptrSize
	local ptrType    = rttiInfo.ptrType
	local ptrLabel   = is64 and "QWORD" or "DWORD"

	-- Back-compat: callers may pass a single objectLocatorAddr (legacy) or a
	-- full objectLocators list. Normalize to a list of {addr, offset} pairs.
	local cols = rttiInfo.objectLocators
	if (not cols or #cols == 0) and rttiInfo.objectLocatorAddr then
		cols = { { addr = rttiInfo.objectLocatorAddr, offset = 0 } }
	end
	if not cols or #cols == 0 then
		log("ERROR: No Object Locators provided to findVTableAndInstances.")
		return nil
	end

	-- VTable layout in memory:
	--   [vTablePtr - ptrSize] = pointer to RTTICompleteObjectLocator
	--   [vTablePtr + 0]       = first virtual function
	--   [vTablePtr + ptrSize] = second virtual function ...
	--
	-- For each candidate COL, scan the module for a pointer-sized value
	-- equal to the COL address. If any are found, it's a real COL and the
	-- matches + ptrSize are the vtable pointers. If none are found, this
	-- was a coincidental signature match and we drop it silently (this is
	-- the etra-style implicit validation -- a COL is real iff something
	-- references it as a pointer).
	--
	-- Classes with multiple inheritance have several COLs (one per subobject
	-- vtable). We union all vtables we discover, deduplicating along the way.
	-- Each vtable inherits the COL.offset of the COL that referenced it --
	-- that's how we tell primary from secondary subobject vtables.
	local vTableList = {}
	local seenVT = {}
	local validatedCOLs = 0
	local droppedCOLs = 0

	for _, col in ipairs(cols) do
		local objLocAddr = col.addr
		local colOffset  = col.offset or 0
		local role = (colOffset == 0) and "primary" or ("secondary +0x" .. string.format("%X", colOffset))

		log("Probing COL " .. hexStr(objLocAddr) .. " (" .. role ..
			") for vtable backrefs as " .. ptrLabel .. " in module...")
		local refs = scanForHexValue(objLocAddr, ptrType, modBase, modEnd, "ObjLocator ptr")

		if #refs == 0 then
			-- No vtable references this address -> coincidental signature
			-- match, not a real COL. Drop silently (counted for summary).
			droppedCOLs = droppedCOLs + 1
			log("  -> no vtable backrefs; dropping (was a coincidental signature match)")
		else
			validatedCOLs = validatedCOLs + 1
			for _, refAddr in ipairs(refs) do
				local vt = refAddr + ptrSize
				if not seenVT[vt] then
					seenVT[vt] = true
					table.insert(vTableList, { vt = vt, offset = colOffset })
					log("  VTable: " .. hexStr(vt) .. "  (" .. role .. ")")
				end
			end
		end
	end

	if droppedCOLs > 0 then
		log(droppedCOLs .. " candidate COL(s) dropped (no vtable backrefs); " ..
			validatedCOLs .. " confirmed real.")
	end

	if #vTableList == 0 then
		log("ERROR: No VTable references found for any Object Locator candidate.")
		log("  All " .. #cols .. " signature-passing candidates lacked vtable backrefs.")
		log("  This class has no live vtable in this module's .rdata. Possible reasons:")
		log("   - Class is abstract / never instantiated, so no concrete vtable exists.")
		log("   - The vtable lives in a different module than the one selected.")
		log("   - Linker stripped the vtable (rare, e.g. LTO with /OPT:REF).")
		return nil
	end

	-- Scan user-mode process memory for instances (pointers to each VTable).
	-- x64 user-mode tops out at 0x7FFFFFFFFFFF.
	-- x86 user-mode is 0x7FFEFFFF for a "small" process, but is 0xFFFEFFFF
	-- for a Large-Address-Aware process running on a 64-bit OS. Virtually
	-- every modern 32-bit game ships LAA so it can address all 4 GB of RAM
	-- for textures/meshes -- and game objects routinely get allocated above
	-- the 2 GB line. We must scan the full 4 GB user range or we'll silently
	-- miss any instance whose heap allocation landed in the upper half.
	-- CE skips unmapped pages, so scanning the full range on a non-LAA
	-- process costs essentially nothing.
	local scanMax = is64 and 0x7FFFFFFFFFFF or 0xFFFFFFFF

	-- Restrict to writable memory: real C++ instances live on the heap, on the
	-- stack, or in .data globals -- all writable. RTTI metadata, vtables, and
	-- code (where false positives come from) sit in read-only .rdata or
	-- executable-only .text and get excluded.
	-- We use "+W" rather than "+W-X" so that custom allocators which mark
	-- pages PAGE_EXECUTE_READWRITE (some engines with embedded scripting/JIT)
	-- aren't excluded. The .text exclusion is still effective because .text is
	-- "-W+X" (writable bit clear).
	-- Heap allocations are pointer-aligned, so requiring ptrSize alignment
	-- discards any stray matches that happened to land mid-structure.
	local instanceScanOpts = {
		protection = "+W",
		alignType  = fsmAligned,
		alignParam = tostring(ptrSize)
	}

	foundInstances = {}
	for _, entry in ipairs(vTableList) do
		local vt = entry.vt
		local vtOffset = entry.offset
		local role = (vtOffset == 0) and "primary" or ("secondary +0x" .. string.format("%X", vtOffset))

		log("Scanning writable memory for instances -> VTable " .. hexStr(vt) ..
			" (" .. role .. ", aligned " .. ptrSize .. ") ...")
		local insts = scanForHexValue(vt, ptrType, 0, scanMax, "Instances", instanceScanOpts)
		for _, addr in ipairs(insts) do
			-- For secondary subobject vtables, the COMPLETE object pointer
			-- lives `vtOffset` bytes before the matched address.
			local completeAddr = (vtOffset > 0) and (addr - vtOffset) or addr
			table.insert(foundInstances, {
				address      = addr,           -- raw scan hit (subobject start)
				vTable       = vt,
				vtOffset     = vtOffset,       -- COL.offset, 0 for primary
				completeAddr = completeAddr    -- start of complete object
			})
		end
	end

	log("Total live instances: " .. #foundInstances)
	if #foundInstances == 0 then
		log("  (zero instances usually means the class isn't currently")
		log("   instantiated -- abstract base, factory-only, or simply")
		log("   not in use in the current game state)")
	end
	return foundInstances
end

-- ============================================================================
-- CORE RTTI LOGIC (REVERSE)
-- ============================================================================


--- Walk backward from an instance pointer through RTTI to recover the type name.
--- Returns a result table on success, or nil + error string on failure. Doesnt' log anything
local function getInstanceTypeData(instanceAddr)
	local is64       = targetIs64Bit()
	local ptrSize    = is64 and 8 or 4
	local nameOffset = is64 and 0x10 or 0x08

	-- Step 1: Read VTable pointer at instance address
	local vftablePtr = readPtr(instanceAddr)
	if not vftablePtr or vftablePtr == 0 then
		return nil, "Failed to read VTable pointer"
	end

	local isPointer = false
	local pointerTarget = nil
	if vftablePtr < moduleBaseAddress or vftablePtr > (moduleBaseAddress + moduleSize) then
		isPointer = true
		pointerTarget = vftablePtr
		vftablePtr = readPtr(vftablePtr)
	end
	if not vftablePtr or vftablePtr == 0 then
		return nil, "Failed to read VTable pointer"
	end
	
	local vtModule = findModuleForAddress(vftablePtr)

	-- Step 2: Read Object Locator pointer at VTable - ptrSize
	local objLocPtrAddr = vftablePtr - ptrSize
	local objLocAddr = readPtr(objLocPtrAddr)
	if not objLocAddr or objLocAddr == 0 then
		return nil, "Failed to read Object Locator pointer"
	end

	-- Step 3: Validate signature
	local signature = readInteger(objLocAddr)
	if signature == nil then
		return nil, "Failed to read Object Locator"
	end

	local expectedSig = is64 and 1 or 0

	-- Step 3b: Read COL.offset (vtable's byte offset within the complete object).
	-- If non-zero, the pointer we were given points to a SECONDARY subobject and
	-- the complete object lives at (instanceAddr - colOffset).
	local colOffset = readDword(objLocAddr + 0x04) or 0
	local completeObjectAddr = instanceAddr - colOffset

	-- Step 4: Resolve TypeDescriptor address
	local typeDescAddr
	local moduleBase

	if is64 then
		-- x64: RVA-based. Use pSelf at +0x14 to compute module base.
		local pSelfRVA = readInteger(objLocAddr + 0x14)
		if not pSelfRVA then
			return nil, "Failed to read pSelf RVA"
		end

		moduleBase = objLocAddr - pSelfRVA
		local typeDescRVA = readInteger(objLocAddr + 0x0C)
		if not typeDescRVA then
			return nil, "Failed to read TypeDescriptor RVA"
		end

		typeDescAddr = moduleBase + typeDescRVA
	else
		-- x86: Direct pointer at +0x0C (read as unsigned to survive high addresses)
		typeDescAddr = readDword(objLocAddr + 0x0C)
		if not typeDescAddr or typeDescAddr == 0 then
			return nil, "Failed to read TypeDescriptor pointer"
		end
	end

	-- Step 5: Read type name string
	local typeName = readString(typeDescAddr + nameOffset, 300, false)
	if not typeName or #typeName == 0 then
		return nil, "Failed to read type name"
	end

	if not typeName:find("^%.%?A[VUW]") then
		return nil, "Typename isn't an RTTI type"
	end

	local demangled = demangleMSVC(typeName)

	return {
		instanceAddr        = instanceAddr,
		vftablePtr          = vftablePtr,
		vftableModule       = vtModule and vtModule.name or "unknown",
		objLocAddr          = objLocAddr,
		signature           = signature,
		colOffset           = colOffset,
		completeObjectAddr  = completeObjectAddr,  -- == instanceAddr when colOffset == 0
		moduleBase          = moduleBase,
		typeDescAddr        = typeDescAddr,
		typeName            = typeName,
		demangled           = demangled,
		pointerTarget		= pointerTarget			-- nil if instanceAddr is the base of an instance type, or an address if instanceAddr is a pointer to an instance type
	}
end


--- Basically the logger for getInstanceTypeData
--- Returns a result table on success, or nil + error string on failure.
local function resolveInstanceType(instanceAddr)
	local result, errorValue = getInstanceTypeData(instanceAddr)

	local is64       = targetIs64Bit()
	local ptrSize    = is64 and 8 or 4
	local nameOffset = is64 and 0x10 or 0x08

	log("========================================")
	log("Reverse RTTI Lookup: " .. hexStr(instanceAddr))
	log("Architecture: " .. (is64 and "x64" or "x86"))
	log("========================================")

	if not result then
           log("Not an RTTI type: " .. errorValue)
           return result, errorValue
	end

	-- Step 1: Read VTable pointer at instance address
	local vftablePtr = result.vftablePtr
	if not vftablePtr or vftablePtr == 0 then
		log("  ERROR: Could not read VTable pointer at " .. hexStr(instanceAddr))
		log("  (address may be invalid or not readable)")
		return nil, "Failed to read VTable pointer"
	end

	local vtModule = result.vttableModule
	if vtModule then
		log("  VTable ptr:         " .. hexStr(vftablePtr) .. "  (module: " .. vtModule.name .. ")")
	else
		log("  VTable ptr:         " .. hexStr(vftablePtr) .. "  (WARNING: not in any known module)")
	end

	-- Step 2: Read Object Locator pointer at VTable - ptrSize
	local objLocPtrAddr = vftablePtr - ptrSize
	local objLocAddr = readPtr(objLocPtrAddr)
	if not objLocAddr or objLocAddr == 0 then
		log("  ERROR: Could not read Object Locator pointer at " .. hexStr(objLocPtrAddr))
		log("  (this may not be an RTTI-enabled class)")
		return nil, "Failed to read Object Locator pointer"
	end
	log("  Object Locator:     " .. hexStr(objLocAddr))

	-- Step 3: Validate signature
	local signature = result.signature
	if signature == nil then
		log("  ERROR: Could not read Object Locator signature at " .. hexStr(objLocAddr))
		return nil, "Failed to read Object Locator"
	end

	local expectedSig = is64 and 1 or 0
	if signature ~= expectedSig then
		log("  Signature:          " .. hexStr(signature) .. "  (UNEXPECTED - expected " .. expectedSig .. ")")
		log("  WARNING: This may not be a valid RTTI Object Locator")
	else
		log("  Signature:          " .. hexStr(signature) .. "  (" .. (is64 and "64-bit" or "32-bit") .. ")")
	end

	-- Step 3b: Read COL.offset (vtable's byte offset within the complete object).
	-- If non-zero, the pointer we were given points to a SECONDARY subobject and
	-- the complete object lives at (instanceAddr - colOffset).
	local colOffset = result.colOffset
	local completeObjectAddr = instanceAddr - colOffset
	if colOffset == 0 then
		log("  COL.offset:         0x0  (primary vtable - this IS the complete object)")
	else
		log("  COL.offset:         " .. hexStr(colOffset) ..
			"  (secondary subobject)")
		log("  Complete object at: " .. hexStr(completeObjectAddr) ..
			"  (= " .. hexStr(instanceAddr) .. " - " .. hexStr(colOffset) .. ")")
	end

	-- Step 4: Resolve TypeDescriptor address
	local typeDescAddr = result.TypeDescAddr
	local moduleBase = result.moduleBase
	log("  Module base:        " .. hexStr(moduleBase))
	log("  TypeDescriptor:     " .. hexStr(typeDescAddr))

	-- Step 5: Read type name string
	local typeName = result.typeName
	log("  Type name:          " .. typeName)

	if not typeName:find("^%.%?A[VUW]") then
		log("  WARNING: Name doesn't match expected RTTI pattern (.?AV / .?AU / .?AW4)")
	end

	local demangled = result.demangled
	if demangled ~= typeName then
		log("  Demangled:          " .. demangled)
	end

	log("========================================")

	return result
end

--- Walk the class hierarchy from an Object Locator to list all base classes.
local function walkClassHierarchy(objLocAddr, moduleBase)
	local is64       = targetIs64Bit()
	local nameOffset = is64 and 0x10 or 0x08

	-- Read pClassHierarchy at +0x10 (RVA for x64, direct pointer for x86)
	local classHierAddr
	if is64 then
		local hierRVA = readInteger(objLocAddr + 0x10)
		if not hierRVA then
			log("  Could not read pClassHierarchy RVA")
			return
		end
		classHierAddr = moduleBase + hierRVA
	else
		classHierAddr = readDword(objLocAddr + 0x10)
		if not classHierAddr or classHierAddr == 0 then
			log("  Could not read pClassHierarchy pointer")
			return
		end
	end

	-- RTTIClassHierarchyDescriptor:
	--   +0x00  signature      (DWORD)
	--   +0x04  attributes     (DWORD) - bit0: multiple inheritance, bit1: virtual inheritance
	--   +0x08  numBaseClasses (DWORD)
	--   +0x0C  pBaseClassArray (DWORD RVA for x64 / direct pointer for x86)
	local attributes     = readInteger(classHierAddr + 0x04)
	local numBaseClasses = readInteger(classHierAddr + 0x08)

	if not numBaseClasses or numBaseClasses == 0 then
		log("  No base classes found")
		return
	end

	local attrStr = ""
	if attributes then
		if (attributes % 2) == 1 then attrStr = attrStr .. " [multiple-inheritance]" end
		if (math.floor(attributes / 2) % 2) == 1 then attrStr = attrStr .. " [virtual-inheritance]" end
	end

	log("  Class hierarchy (" .. numBaseClasses .. " entries):" .. attrStr)

	local baseClassArrayAddr
	if is64 then
		local arrRVA = readInteger(classHierAddr + 0x0C)
		if not arrRVA then return end
		baseClassArrayAddr = moduleBase + arrRVA
	else
		baseClassArrayAddr = readDword(classHierAddr + 0x0C)
		if not baseClassArrayAddr or baseClassArrayAddr == 0 then return end
	end

	local maxEntries = math.min(numBaseClasses, 50)

	for i = 0, maxEntries - 1 do
		local bcDescAddr
		if is64 then
			local bcRVA = readInteger(baseClassArrayAddr + (i * 4))
			if not bcRVA then break end
			bcDescAddr = moduleBase + bcRVA
		else
			bcDescAddr = readDword(baseClassArrayAddr + (i * 4))
			if not bcDescAddr or bcDescAddr == 0 then break end
		end

		-- RTTIBaseClassDescriptor +0x00 = pTypeDescriptor (RVA for x64 / direct for x86)
		local baseTypeDescAddr
		if is64 then
			local tdRVA = readInteger(bcDescAddr)
			if not tdRVA then break end
			baseTypeDescAddr = moduleBase + tdRVA
		else
			baseTypeDescAddr = readDword(bcDescAddr)
			if not baseTypeDescAddr or baseTypeDescAddr == 0 then break end
		end

		local baseName = readString(baseTypeDescAddr + nameOffset, 300, false)
		if baseName and #baseName > 0 then
			local demangled = demangleMSVC(baseName)
			local prefix = (i == 0) and "    -> " or "       "
			local label  = (i == 0) and "(this class)" or "(base " .. i .. ")"
			log(prefix .. demangled .. "  " .. label)
		end
	end
end

--- Full reverse lookup: resolve type + walk hierarchy
local function fullReverseLookup(instanceAddr, range)
	range = range or 0		-- default if not specified.
	refreshModuleCache()

	local isRange = range > 0
	if isRange then
		local endAddr = instanceAddr + range
		log("========================================")
		log("Reverse RTTI Lookup of range: " .. hexStr(instanceAddr) .. " - " .. hexStr(endAddr))
		log("========================================")

		local currentAddr = instanceAddr
		while currentAddr <= endAddr do
			local result = getInstanceTypeData(currentAddr, range)
			if result then
				local isPointerStr = " -> "
				if result.pointerTarget ~= nil then
					isPointerStr = string.format(" (Pointer to %s) -> ", hexStr(result.pointerTarget))
				end
				local offset = currentAddr - instanceAddr
				log(hexStr(currentAddr) .. string.format(" (Offset: %s)", hexStr(offset)) .. isPointerStr .. result.demangled .. " (" .. result.typeName .. ")")
			end
			currentAddr = currentAddr + 8
		end
	else
		local result = resolveInstanceType(instanceAddr)
		if result then
			walkClassHierarchy(result.objLocAddr, result.moduleBase)
			log("")
		end
	end
	return result
end

-- ============================================================================
-- VTABLE / DISASSEMBLY LOGIC
-- ============================================================================

--- Read all virtual function pointers from an instance's vtable
local function readVTableFunctions(instanceAddr)
	vtableFunctions = {}
	if not moduleCache then refreshModuleCache() end

	local ptrSize = targetIs64Bit() and 8 or 4
	local vtPtr = readPtr(instanceAddr)
	if not vtPtr or vtPtr == 0 then
		log("ERROR: Cannot read vtable pointer at " .. hexStr(instanceAddr))
		return vtableFunctions
	end

	local vtMod = findModuleForAddress(vtPtr)
	if vtMod then
		log("VTable at " .. hexStr(vtPtr) .. "  (module: " .. vtMod.name .. ")")
	else
		log("WARNING: VTable pointer " .. hexStr(vtPtr) .. " not in any known module")
	end

	local MAX_ENTRIES = 500
	for i = 0, MAX_ENTRIES - 1 do
		local entryAddr = vtPtr + (i * ptrSize)
		local funcAddr = readPtr(entryAddr)

		if not funcAddr or funcAddr == 0 then break end

		local funcMod = findModuleForAddress(funcAddr)
		if not funcMod then
			log("  VTable walk stopped at [" .. i .. "]: " .. hexStr(funcAddr) .. " (not in any known module)")
			break
		end

		local symName = getNameFromAddress(funcAddr)
		local name = (symName and symName ~= "") and symName or hexStr(funcAddr)
		table.insert(vtableFunctions, {
			index = i,
			address = funcAddr,
			name = name
		})

		if i % 100 == 0 then processMessages() end
	end

	log("VTable: " .. #vtableFunctions .. " function(s)")
	return vtableFunctions
end

--- Disassemble a full function starting at funcAddr.
--- Returns table of formatted lines matching CE memory viewer format:
---   MODULE+OFFSET - BYTES - INSTRUCTION
local function disassembleFunction(funcAddr)
	local lines = {}
	local currentAddr = funcAddr
	local MAX_INSTRUCTIONS = 2000

	for i = 1, MAX_INSTRUCTIONS do
		local instrSize = getInstructionSize(currentAddr)
		if not instrSize or instrSize == 0 then break end

		-- Read raw bytes directly for reliable formatting
		local rawBytes = readBytes(currentAddr, instrSize, true)
		local byteStr = ""
		if rawBytes then
			local byteParts = {}
			for b = 1, #rawBytes do
				byteParts[#byteParts + 1] = string.format("%02X", rawBytes[b] or 0)
			end
			byteStr = table.concat(byteParts, " ")
		end

		-- Get instruction text from CE's disassembler
		local raw = disassemble(currentAddr)
		if not raw or raw == "" then break end

		-- Extract instruction mnemonic from CE output
		-- CE format: "HEXADDR - BYTES - INSTRUCTION"
		-- Split on first two " - " delimiters; dashes in operands are preserved
		local instrPart = raw
		local pos1 = raw:find(" %- ")
		if pos1 then
			local rest = raw:sub(pos1 + 3)
			local pos2 = rest:find(" %- ")
			if pos2 then
				instrPart = rest:sub(pos2 + 3)
			else
				instrPart = rest
			end
		end
		instrPart = instrPart:match("^%s*(.-)%s*$") or instrPart

		-- Resolve address to MODULE+OFFSET format
		local resolvedName = getNameFromAddress(currentAddr) or hexStr(currentAddr)

		local line = string.format("%s - %-24s- %s", resolvedName, byteStr, instrPart)
		table.insert(lines, line)

		-- Stop at function boundaries
		local instrLower = instrPart:lower()
		if instrLower:match("^ret") or instrLower:match("^int%s*3") then break end

		currentAddr = currentAddr + instrSize

		if i % 200 == 0 then processMessages() end
	end

	return lines
end

--- Batch-disassemble the first N vtable functions for an instance.
--- Reads the vtable (if not already loaded), then disassembles each function
--- and returns a single table of formatted lines with separator headers.
local function dumpVTableDisasm(instanceAddr, maxFuncs)
	maxFuncs = maxFuncs or 30

	-- Ensure vtable is loaded
	if #vtableFunctions == 0 then
		if not moduleCache then refreshModuleCache() end
		readVTableFunctions(instanceAddr)
	end

	if #vtableFunctions == 0 then
		log("ERROR: No vtable functions found for " .. hexStr(instanceAddr))
		return {}
	end

	local count = math.min(maxFuncs, #vtableFunctions)
	local allLines = {}
	local sep = string.rep("=", 80)

	for idx = 1, count do
		local vf = vtableFunctions[idx]

		table.insert(allLines, sep)
		table.insert(allLines, string.format("[%d]  %s", vf.index, vf.name))
		table.insert(allLines, sep)

		local funcLines = disassembleFunction(vf.address)
		for _, line in ipairs(funcLines) do
			table.insert(allLines, line)
		end
		table.insert(allLines, "")

		log("  Disassembled [" .. vf.index .. "] " .. vf.name .. " (" .. #funcLines .. " instructions)")
		processMessages()
	end

	return allLines
end

-- ============================================================================
-- UI CONSTRUCTION
-- ============================================================================

local function buildUI()
	if mainForm then
		mainForm.destroy()
		mainForm = nil
	end

	mainForm = createForm(false)
	mainForm.Caption = SCRIPT_TITLE
	mainForm.Width = 1350
	mainForm.Height = 780
	mainForm.Position = "poScreenCenter"
	mainForm.BorderStyle = "bsSizeable"

	-- ======== Top panel: Module selection & actions ========
	local panelTop = createPanel(mainForm)
	panelTop.Align = alTop
	panelTop.Height = 35
	panelTop.BevelOuter = bvNone
	panelTop.Caption = ""

	local lblModule = createLabel(panelTop)
	lblModule.Left = 10; lblModule.Top = 16
	lblModule.Caption = "Target Module:"

	mainForm.cbModule = createComboBox(panelTop)
	mainForm.cbModule.Left = 105; mainForm.cbModule.Top = 12
	mainForm.cbModule.Width = 300
	mainForm.cbModule.Style = csDropDownList

	local btnRefreshMods = createButton(panelTop)
	btnRefreshMods.Left = 415; btnRefreshMods.Top = 10
	btnRefreshMods.Width = 80; btnRefreshMods.Caption = "Refresh"
	btnRefreshMods.OnClick = function()
		mainForm.cbModule.Items.Clear()
		local mods = getModuleList()
		for _, m in ipairs(mods) do
			mainForm.cbModule.Items.Add(m.name)
		end
		if mainForm.cbModule.Items.Count > 0 then
			mainForm.cbModule.ItemIndex = 0
		end
		log("Loaded " .. #mods .. " modules")
	end

	local btnScan = createButton(panelTop)
	btnScan.Left = 505; btnScan.Top = 10
	btnScan.Width = 130; btnScan.Caption = "Scan for RTTI"
	btnScan.OnClick = function()
		local idx = mainForm.cbModule.ItemIndex
		if idx < 0 then
			log("ERROR: Select a target module first.")
			return
		end
		targetModule = mainForm.cbModule.Items[idx]
		mainForm.listTypes.Items.Clear()
		mainForm.listInstances.Items.Clear()
		foundInstances = {}

		-- Clear vtable/disasm state
		vtableFunctions = {}
		selectedInstanceAddr = nil
		mainForm.listVTableFuncs.Items.Clear()
		mainForm.memoDisasm.Lines.Clear()

		local ok = scanForRTTITypes(targetModule)
		if ok then
			mainForm.listTypes.Items.BeginUpdate()
			for _, t in ipairs(rttiTypes) do
				mainForm.listTypes.Items.Add(t.name .. "  [" .. hexStr(t.address) .. "]")
			end
			mainForm.listTypes.Items.EndUpdate()
			mainForm.lblTypeCount.Caption = "Types: " .. #rttiTypes
		end
	end

	local btnExport = createButton(panelTop)
	btnExport.Left = 645; btnExport.Top = 10
	btnExport.Width = 120; btnExport.Caption = "Export to Clipboard"
	btnExport.OnClick = function()
		if #filteredTypes == 0 then
			log("Nothing to export.")
			return
		end
		local lines = {}
		for _, t in ipairs(filteredTypes) do
			lines[#lines + 1] = hexStr(t.address) .. "  " .. t.name
		end
		writeToClipboard(table.concat(lines, "\n"))
		log("Exported " .. #filteredTypes .. " types to clipboard.")
	end

	-- ======== Filter bar ========
	local panelFilter = createPanel(mainForm)
	panelFilter.Align = alTop
        panelFilter.Top = panelTop.Height
	panelFilter.Height = 35
	panelFilter.BevelOuter = bvNone
	panelFilter.Caption = ""

	local lblFilter = createLabel(panelFilter)
	lblFilter.Left = 10; lblFilter.Top = 12
	lblFilter.Caption = "Filter:"

	mainForm.edFilter = createEdit(panelFilter)
	mainForm.edFilter.Left = 55; mainForm.edFilter.Top = 8
	mainForm.edFilter.Width = 350

	local btnFilter = createButton(panelFilter)
	btnFilter.Left = 415; btnFilter.Top = 8
	btnFilter.Width = 80; btnFilter.Caption = "Apply"
	btnFilter.OnClick = function()
		local pattern = mainForm.edFilter.Text:lower()
		mainForm.listTypes.Items.BeginUpdate()
		mainForm.listTypes.Items.Clear()
		filteredTypes = {}
		for _, t in ipairs(rttiTypes) do
			if pattern == "" or t.name:lower():find(pattern, 1, true) then
				table.insert(filteredTypes, t)
				mainForm.listTypes.Items.Add(t.name .. "  [" .. hexStr(t.address) .. "]")
			end
		end
		mainForm.listTypes.Items.EndUpdate()
		mainForm.lblTypeCount.Caption = "Types: " .. #filteredTypes .. " / " .. #rttiTypes
		log("Filter: " .. #filteredTypes .. " match '" .. mainForm.edFilter.Text .. "'")
	end

	mainForm.lblTypeCount = createLabel(panelFilter)
	mainForm.lblTypeCount.Left = 510; mainForm.lblTypeCount.Top = 12
	mainForm.lblTypeCount.Caption = "Types: 0"

	-- ======== Reverse lookup bar ========
	local panelReverse = createPanel(mainForm)
	panelReverse.Align = alTop
	panelReverse.Height = 40
        panelReverse.Top = panelFilter.Height + panelTop.Height
	panelReverse.BevelOuter = bvNone
	panelReverse.Caption = ""

	local lblReverse = createLabel(panelReverse)
	lblReverse.Left = 10; lblReverse.Top = 12
	lblReverse.Caption = "Instance Lookup:"

	mainForm.edInstanceAddr = createEdit(panelReverse)
	mainForm.edInstanceAddr.Left = 120; mainForm.edInstanceAddr.Top = 8
	mainForm.edInstanceAddr.Width = 220
	mainForm.edInstanceAddr.Font.Name = "Consolas"
	mainForm.edInstanceAddr.Font.Size = 10

	local function parseReverseLookupAddr()
		local text = mainForm.edInstanceAddr.Text:gsub("%s", "")
		if text == "" then
			log("ERROR: Enter an instance address first.")
			return nil
		end
		text = text:gsub("^0[xX]", "")
		local addr = tonumber(text, 16)
		if not addr then
			log("ERROR: Invalid hex address: " .. mainForm.edInstanceAddr.Text)
			return nil
		end
		return addr
	end

	local btnIdentify = createButton(panelReverse)
	btnIdentify.Left = 350; btnIdentify.Top = 8
	btnIdentify.Width = 110; btnIdentify.Caption = "Identify Type"
	btnIdentify.OnClick = function()
		local addr = parseReverseLookupAddr()
		if addr then fullReverseLookup(addr) end
	end

	local btnIdentifyBrowse = createButton(panelReverse)
	btnIdentifyBrowse.Left = 470; btnIdentifyBrowse.Top = 8
	btnIdentifyBrowse.Width = 130; btnIdentifyBrowse.Caption = "Identify + Browse"
	btnIdentifyBrowse.OnClick = function()
		local addr = parseReverseLookupAddr()
		if not addr then return end
		local result = fullReverseLookup(addr)
		if result then
			-- If the pasted address was a secondary subobject pointer, jump
			-- to the COMPLETE object start so the memory view shows the
			-- whole layout. completeObjectAddr falls back to addr for primary.
			local browseAddr = result.completeObjectAddr or addr
			local mv = getMemoryViewForm()
			mv.HexadecimalView.Address = browseAddr
			mv.show()
		end
	end

	-- Enter key in reverse lookup field triggers identify
	mainForm.edInstanceAddr.OnKeyDown = function(sender, key)
		if key == VK_RETURN then
			btnIdentify.OnClick()
			return 0
		end
		return key
	end

	local function parseReverseLookupRange()
		local text = mainForm.edInstanceRange.Text:gsub("%s", "")
		if text == "" then
			log("ERROR: Enter a range first.")
			return nil
		end
		text = text:gsub("^0[xX]", "")
		local range = tonumber(text, 16)
		if not range then
			log("ERROR: Invalid hex range: " .. mainForm.edInstanceRange.Text)
			return nil
		end
		return range
	end

	local lblRange = createLabel(panelReverse)
	lblRange.Left = btnIdentifyBrowse.Left + btnIdentifyBrowse.Width + 20;
        lblRange.Top = 12
	lblRange.Caption = "Range:"

	mainForm.edInstanceRange = createEdit(panelReverse)
	mainForm.edInstanceRange.Left = lblRange.Left + lblRange.Width - 20;
        mainForm.edInstanceRange.Top = 8
	mainForm.edInstanceRange.Width = 100
	mainForm.edInstanceRange.Font.Name = "Consolas"
	mainForm.edInstanceRange.Font.Size = 10

	local btnRangeIdentify = createButton(panelReverse)
	btnRangeIdentify.Left = mainForm.edInstanceRange.Left + mainForm.edInstanceRange.Width + 5;
        btnRangeIdentify.Top = 8
	btnRangeIdentify.Width = 110; btnRangeIdentify.Caption = "Identify range"
	btnRangeIdentify.OnClick = function()
		local range = parseReverseLookupRange()
                local addr = parseReverseLookupAddr()
		if range and range then fullReverseLookup(addr, range) end
	end

	-- ======== Body: Three-column layout ========
	local panelBody = createPanel(mainForm)
	panelBody.Align = alClient
	panelBody.BevelOuter = bvNone
	panelBody.Caption = ""

	-- === Left column: RTTI Types ===
	local panelLeft = createPanel(panelBody)
	panelLeft.Align = alLeft
	panelLeft.Width = 400
	panelLeft.BevelOuter = bvNone
	panelLeft.Caption = ""

	local lblTypes = createLabel(panelLeft)
	lblTypes.Align = alTop
	lblTypes.Height = 20
	lblTypes.Caption = "  RTTI Types (double-click to trace):"

	mainForm.listTypes = createListBox(panelLeft)
	mainForm.listTypes.Align = alClient
	mainForm.listTypes.MultiSelect = true
	mainForm.listTypes.ExtendedSelect = true

	-- Copy context menu for types
	local popupTypes = createPopupMenu(mainForm)
	local miCopyTypes = createMenuItem(popupTypes)
	miCopyTypes.Caption = "Copy Selected"
	miCopyTypes.OnClick = function()
		local lines = {}
		for i = 0, mainForm.listTypes.Items.Count - 1 do
			if mainForm.listTypes.Selected[i] then
				lines[#lines + 1] = mainForm.listTypes.Items[i]
			end
		end
		if #lines > 0 then
			writeToClipboard(table.concat(lines, "\n"))
			log("Copied " .. #lines .. " type(s) to clipboard.")
		end
	end
	popupTypes.Items.Add(miCopyTypes)

	local miCopyAllTypes = createMenuItem(popupTypes)
	miCopyAllTypes.Caption = "Copy All"
	miCopyAllTypes.OnClick = function()
		local lines = {}
		for i = 0, mainForm.listTypes.Items.Count - 1 do
			lines[#lines + 1] = mainForm.listTypes.Items[i]
		end
		if #lines > 0 then
			writeToClipboard(table.concat(lines, "\n"))
			log("Copied all " .. #lines .. " type(s) to clipboard.")
		end
	end
	popupTypes.Items.Add(miCopyAllTypes)
	mainForm.listTypes.PopupMenu = popupTypes

	-- Double-click a type -> full RTTI chain walk
	mainForm.listTypes.OnDblClick = function()
		local idx = mainForm.listTypes.ItemIndex
		if idx < 0 or idx >= #filteredTypes then return end
		if not targetModule then
			log("ERROR: No module selected.")
			return
		end

		local sel = filteredTypes[idx + 1]
		log("========================================")
		log("Tracing: " .. sel.name)
		log("========================================")
		mainForm.listInstances.Items.Clear()
		foundInstances = {}

		-- Clear vtable/disasm state
		vtableFunctions = {}
		selectedInstanceAddr = nil
		mainForm.listVTableFuncs.Items.Clear()
		mainForm.memoDisasm.Lines.Clear()

		local rttiInfo = findObjectLocator(sel.name, targetModule)
		if not rttiInfo then
			log("FAILED at Object Locator stage.")
			return
		end

		local insts = findVTableAndInstances(rttiInfo)
		if insts and #insts > 0 then
			mainForm.listInstances.Items.BeginUpdate()
			for i, inst in ipairs(insts) do
				-- For primary subobject hits (vtOffset == 0) the scan address
				-- IS the complete object start; show one address.
				-- For secondary subobjects, show "complete <- (sub +off)" so
				-- the user knows the canonical complete-object address while
				-- still seeing where the raw vtable pointer landed.
				local line
				if inst.vtOffset == 0 then
					line = "#" .. i .. "  " .. hexStr(inst.completeAddr) ..
						"  (VT: " .. hexStr(inst.vTable) .. ")"
				else
					line = "#" .. i .. "  " .. hexStr(inst.completeAddr) ..
						"  (VT: " .. hexStr(inst.vTable) ..
						", sub@" .. hexStr(inst.address) ..
						" +0x" .. string.format("%X", inst.vtOffset) .. ")"
				end
				mainForm.listInstances.Items.Add(line)
			end
			mainForm.listInstances.Items.EndUpdate()
			log("Done: " .. #insts .. " instance(s)")
		else
			log("Done: no live instances found.")
		end
	end

	-- === Middle column: Instances + Buttons + Log ===
	local panelMiddle = createPanel(panelBody)
	panelMiddle.Align = alLeft
	panelMiddle.Width = 500
        panelMiddle.Left = panelLeft.Width
	panelMiddle.BevelOuter = bvNone
	panelMiddle.Caption = ""

        -- Splitter between left and middle column
	local splitterLeftMiddle = createSplitter(panelBody)
        splitterLeftMiddle.Left = panelLeft.Width

	-- --- Instances list (top) ---
	local panelInstTop = createPanel(panelMiddle)
	panelInstTop.Align = alTop
	panelInstTop.Height = 220
	panelInstTop.BevelOuter = bvNone
	panelInstTop.Caption = ""

	local lblInst = createLabel(panelInstTop)
	lblInst.Left = 5; lblInst.Top = 2
	lblInst.Caption = "Instances (click -> VTable | dbl-click -> Memory):"

	mainForm.listInstances = createListBox(panelInstTop)
	mainForm.listInstances.Left = 5; mainForm.listInstances.Top = 20
	mainForm.listInstances.Anchors = "[akLeft,akTop,akRight,akBottom]"
	mainForm.listInstances.Width = panelInstTop.Width - 10
	mainForm.listInstances.Height = panelInstTop.Height - 25
	mainForm.listInstances.MultiSelect = true
	mainForm.listInstances.ExtendedSelect = true

	-- Context menu for instances (copy + identify)
	local popupInst = createPopupMenu(mainForm)

	local miIdentifyInst = createMenuItem(popupInst)
	miIdentifyInst.Caption = "Identify Type"
	miIdentifyInst.OnClick = function()
		local idx = mainForm.listInstances.ItemIndex
		if idx < 0 or idx >= #foundInstances then
			log("Select an instance first."); return
		end
		local inst = foundInstances[idx + 1]
		-- Use completeAddr so reverse lookup walks the PRIMARY vtable and
		-- reports the complete class. For secondary subobject hits this also
		-- demonstrates that the offset arithmetic landed on a real object.
		fullReverseLookup(inst.completeAddr or inst.address)
	end
	popupInst.Items.Add(miIdentifyInst)

	local miSep = createMenuItem(popupInst)
	miSep.Caption = "-"
	popupInst.Items.Add(miSep)

	local miCopyInst = createMenuItem(popupInst)
	miCopyInst.Caption = "Copy Selected"
	miCopyInst.OnClick = function()
		local lines = {}
		for i = 0, mainForm.listInstances.Items.Count - 1 do
			if mainForm.listInstances.Selected[i] then
				lines[#lines + 1] = mainForm.listInstances.Items[i]
			end
		end
		if #lines > 0 then
			writeToClipboard(table.concat(lines, "\n"))
			log("Copied " .. #lines .. " instance(s) to clipboard.")
		end
	end
	popupInst.Items.Add(miCopyInst)

	local miCopyAllInst = createMenuItem(popupInst)
	miCopyAllInst.Caption = "Copy All"
	miCopyAllInst.OnClick = function()
		local lines = {}
		for i = 0, mainForm.listInstances.Items.Count - 1 do
			lines[#lines + 1] = mainForm.listInstances.Items[i]
		end
		if #lines > 0 then
			writeToClipboard(table.concat(lines, "\n"))
			log("Copied all " .. #lines .. " instance(s) to clipboard.")
		end
	end
	popupInst.Items.Add(miCopyAllInst)
	mainForm.listInstances.PopupMenu = popupInst

	-- Instance double-click -> open Memory Browser at the COMPLETE object start
	-- (for secondary subobjects this is inst.completeAddr, which is the actual
	-- whole-object pointer; opening at inst.address would land mid-object).
	mainForm.listInstances.OnDblClick = function()
		local idx = mainForm.listInstances.ItemIndex
		if idx < 0 or idx >= #foundInstances then return end
		local inst = foundInstances[idx + 1]
		local browseAddr = inst.completeAddr or inst.address
		if inst.vtOffset and inst.vtOffset > 0 then
			log("Browsing memory at " .. hexStr(browseAddr) ..
				"  (complete object; subobject vtable was at +0x" ..
				string.format("%X", inst.vtOffset) .. ")")
		else
			log("Browsing memory at " .. hexStr(browseAddr))
		end
		local mv = getMemoryViewForm()
		mv.HexadecimalView.Address = browseAddr
		mv.show()
	end

	-- Instance single-click -> populate VTable functions list
	-- Use inst.address (the subobject vtable pointer slot) so the listed
	-- functions match the vtable that was actually matched in this hit, not
	-- the primary class vtable that lives at completeAddr.
	mainForm.listInstances.OnClick = function()
		local idx = mainForm.listInstances.ItemIndex
		if idx < 0 or idx >= #foundInstances then return end

		local inst = foundInstances[idx + 1]
		if inst.address == selectedInstanceAddr then return end
		selectedInstanceAddr = inst.address

		mainForm.listVTableFuncs.Items.Clear()
		mainForm.memoDisasm.Lines.Clear()
		vtableFunctions = {}

		if not moduleCache then refreshModuleCache() end

		readVTableFunctions(inst.address)

		mainForm.listVTableFuncs.Items.BeginUpdate()
		for _, vf in ipairs(vtableFunctions) do
			mainForm.listVTableFuncs.Items.Add(
				string.format("[%d]  %s", vf.index, vf.name))
		end
		mainForm.listVTableFuncs.Items.EndUpdate()
	end

	-- --- Action buttons (middle) ---
	local panelButtons = createPanel(panelMiddle)
	panelButtons.Align = alTop
	panelButtons.Height = 70
	panelButtons.BevelOuter = bvNone
	panelButtons.Caption = ""

	local btnAddToTable = createButton(panelButtons)
	btnAddToTable.Left = 5; btnAddToTable.Top = 5
	btnAddToTable.Width = 130; btnAddToTable.Caption = "Add to CE Table"
	btnAddToTable.OnClick = function()
		if #foundInstances == 0 then
			log("No instances to add."); return
		end
		local al = getAddressList()
		local parent = al.createMemoryRecord()
		parent.Description = "[RTTI] Instances"
		parent.IsGroupHeader = true
		parent.Options = "[moHideChildren]"

		local entryType = targetIs64Bit() and vtQword or vtDword
		for i, inst in ipairs(foundInstances) do
			local mr = al.createMemoryRecord()
			-- Address the COMPLETE object so dissection/structure walking
			-- starts at the real object base, not at a multi-inherited slice.
			local mrAddr = inst.completeAddr or inst.address
			local desc
			if inst.vtOffset and inst.vtOffset > 0 then
				desc = "#" .. i .. " VT:" .. hexStr(inst.vTable) ..
					" (complete; sub@" .. hexStr(inst.address) ..
					" +0x" .. string.format("%X", inst.vtOffset) .. ")"
			else
				desc = "#" .. i .. " VT:" .. hexStr(inst.vTable)
			end
			mr.Description = desc
			mr.Address = string.format("%X", mrAddr)
			mr.Type = entryType
			mr.appendToEntry(parent)
		end
		log("Added " .. #foundInstances .. " entries to address list.")
	end

	local btnDissect = createButton(panelButtons)
	btnDissect.Left = 145; btnDissect.Top = 5
	btnDissect.Width = 130; btnDissect.Caption = "Dissect Structure"
	btnDissect.OnClick = function()
		local idx = mainForm.listInstances.ItemIndex
		if idx < 0 or idx >= #foundInstances then
			log("Select an instance first."); return
		end
		local inst = foundInstances[idx + 1]
		-- Dissect from the COMPLETE object start so the layout walker sees
		-- the full inheritance chain rather than starting partway in.
		local dissectAddr = inst.completeAddr or inst.address
		log("Dissecting structure at " .. hexStr(dissectAddr) ..
			((inst.vtOffset and inst.vtOffset > 0) and
				("  (sub@" .. hexStr(inst.address) ..
				 " +0x" .. string.format("%X", inst.vtOffset) .. ")") or ""))

		local mv = getMemoryViewForm()
		mv.HexadecimalView.Address = dissectAddr
		mv.show()
		mv.BringToFront()

		local t = createTimer(nil, false)
		t.Interval = 300
		t.OnTimer = function(sender)
			sender.Enabled = false
			sender.destroy()
			keyDown(VK_CONTROL)
			keyDown(VK_D)
			keyUp(VK_D)
			keyUp(VK_CONTROL)
		end
		t.Enabled = true
	end

	local btnDump = createButton(panelButtons)
	btnDump.Left = 285; btnDump.Top = 5
	btnDump.Width = 100; btnDump.Caption = "Hex Dump"
	btnDump.OnClick = function()
		local idx = mainForm.listInstances.ItemIndex
		if idx < 0 or idx >= #foundInstances then
			log("Select an instance first."); return
		end
		local inst = foundInstances[idx + 1]
		-- Hex dump from the COMPLETE object base for the same reason
		local base = inst.completeAddr or inst.address
		if inst.vtOffset and inst.vtOffset > 0 then
			log("--- Hex dump: " .. hexStr(base) ..
				" (256 bytes; complete object, sub@" .. hexStr(inst.address) ..
				" +0x" .. string.format("%X", inst.vtOffset) .. ") ---")
		else
			log("--- Hex dump: " .. hexStr(base) .. " (256 bytes) ---")
		end

		for off = 0, 0xF0, 0x10 do
			local hexPart = ""
			local asciiPart = ""
			local bytes = readBytes(base + off, 16, true)
			if bytes then
				for b = 1, 16 do
					local v = bytes[b]
					if v then
						hexPart = hexPart .. string.format("%02X ", v)
						asciiPart = asciiPart .. (v >= 32 and v <= 126 and string.char(v) or ".")
					else
						hexPart = hexPart .. "?? "
						asciiPart = asciiPart .. "?"
					end
				end
			else
				hexPart = string.rep("?? ", 16)
				asciiPart = string.rep("?", 16)
			end
			log(string.format("+0x%03X: %s| %s", off, hexPart, asciiPart))
		end
		log("--- End dump ---")
	end

	local btnDumpVTable = createButton(panelButtons)
	btnDumpVTable.Left = 5; btnDumpVTable.Top = 38
	btnDumpVTable.Width = 160; btnDumpVTable.Caption = "Dump VTable Disasm"
	btnDumpVTable.OnClick = function()
		local idx = mainForm.listInstances.ItemIndex
		if idx < 0 or idx >= #foundInstances then
			log("Select an instance first."); return
		end
		local inst = foundInstances[idx + 1]
		local role = (inst.vtOffset and inst.vtOffset > 0)
			and (" (secondary subobject vtable @ +0x" .. string.format("%X", inst.vtOffset) .. ")")
			or " (primary vtable)"
		log("Dumping VTable disassembly for " .. hexStr(inst.address) ..
			role .. " - first 30 functions...")

		local allLines = dumpVTableDisasm(inst.address, 30)
		if #allLines > 0 then
			writeToClipboard(table.concat(allLines, "\n"))
			log("Copied " .. #allLines .. " lines to clipboard (" .. math.min(30, #vtableFunctions) .. " functions)")
		else
			log("No disassembly generated.")
		end
	end

	-- --- Log (fills remaining middle space) ---
	local lblLog = createLabel(panelMiddle)
	lblLog.Align = alTop
	lblLog.Caption = "  Log:"
	lblLog.Height = 18

	mainForm.memoLog = createMemo(panelMiddle)
	mainForm.memoLog.Align = alClient
	mainForm.memoLog.ReadOnly = true
	mainForm.memoLog.WordWrap = false
	mainForm.memoLog.ScrollBars = ssBoth
	mainForm.memoLog.Font.Name = "Consolas"
	mainForm.memoLog.Font.Size = 9

	-- === Right column: VTable Functions + Disassembly ===
	local panelVTable = createPanel(panelBody)
	panelVTable.Align = alClient
	panelVTable.BevelOuter = bvNone
	panelVTable.Caption = ""

	-- Splitter between middle and right columns
	local splitterMiddleRight = createSplitter(panelBody)
	splitterMiddleRight.Left = panelMiddle.Width

	-- --- VTable functions list (top half) ---
	local panelVTFuncs = createPanel(panelVTable)
	panelVTFuncs.Align = alTop
	panelVTFuncs.Height = 280
	panelVTFuncs.BevelOuter = bvNone
	panelVTFuncs.Caption = ""

	local lblVTFuncs = createLabel(panelVTFuncs)
	lblVTFuncs.Align = alTop
	lblVTFuncs.Height = 20
	lblVTFuncs.Caption = "  VTable Functions (click -> disassembly | dbl-click -> CE disassembler):"

	mainForm.listVTableFuncs = createListBox(panelVTFuncs)
	mainForm.listVTableFuncs.Align = alClient
	mainForm.listVTableFuncs.Font.Name = "Consolas"
	mainForm.listVTableFuncs.Font.Size = 9

	-- VTable function click -> show disassembly in bottom pane
	mainForm.listVTableFuncs.OnClick = function()
		local idx = mainForm.listVTableFuncs.ItemIndex
		if idx < 0 or idx >= #vtableFunctions then return end

		local vf = vtableFunctions[idx + 1]
		mainForm.memoDisasm.Lines.Clear()

		log("Disassembling [" .. vf.index .. "] " .. vf.name .. " ...")

		local disasmLines = disassembleFunction(vf.address)

		mainForm.memoDisasm.Lines.BeginUpdate()
		for _, line in ipairs(disasmLines) do
			mainForm.memoDisasm.Lines.Add(line)
		end
		mainForm.memoDisasm.Lines.EndUpdate()

		mainForm.memoDisasm.SelStart = 0
		mainForm.memoDisasm.SelLength = 0

		log("Disassembly: " .. #disasmLines .. " instruction(s)")
	end

	-- VTable function double-click -> open in CE's disassembler
	mainForm.listVTableFuncs.OnDblClick = function()
		local idx = mainForm.listVTableFuncs.ItemIndex
		if idx < 0 or idx >= #vtableFunctions then return end
		local vf = vtableFunctions[idx + 1]
		log("Opening CE disassembler at " .. vf.name)
		local mv = getMemoryViewForm()
		mv.DisassemblerView.SelectedAddress = vf.address
		mv.show()
	end

	-- Context menu for VTable functions
	local popupVT = createPopupMenu(mainForm)

	local miCopyAddr = createMenuItem(popupVT)
	miCopyAddr.Caption = "Copy Address"
	miCopyAddr.OnClick = function()
		local idx = mainForm.listVTableFuncs.ItemIndex
		if idx < 0 or idx >= #vtableFunctions then return end
		writeToClipboard(hexStr(vtableFunctions[idx + 1].address))
		log("Copied address to clipboard.")
	end
	popupVT.Items.Add(miCopyAddr)

	local miCopyName = createMenuItem(popupVT)
	miCopyName.Caption = "Copy Name"
	miCopyName.OnClick = function()
		local idx = mainForm.listVTableFuncs.ItemIndex
		if idx < 0 or idx >= #vtableFunctions then return end
		writeToClipboard(vtableFunctions[idx + 1].name)
		log("Copied name to clipboard.")
	end
	popupVT.Items.Add(miCopyName)

	local miSepVT1 = createMenuItem(popupVT)
	miSepVT1.Caption = "-"
	popupVT.Items.Add(miSepVT1)

	local miCopyAllVT = createMenuItem(popupVT)
	miCopyAllVT.Caption = "Copy All Functions"
	miCopyAllVT.OnClick = function()
		local lines = {}
		for _, vf in ipairs(vtableFunctions) do
			lines[#lines + 1] = string.format("[%d]  %s  (%s)", vf.index, vf.name, hexStr(vf.address))
		end
		if #lines > 0 then
			writeToClipboard(table.concat(lines, "\n"))
			log("Copied " .. #lines .. " vtable function(s) to clipboard.")
		end
	end
	popupVT.Items.Add(miCopyAllVT)

	local miSepVT2 = createMenuItem(popupVT)
	miSepVT2.Caption = "-"
	popupVT.Items.Add(miSepVT2)

	local miBrowseHex = createMenuItem(popupVT)
	miBrowseHex.Caption = "Browse in Memory Viewer"
	miBrowseHex.OnClick = function()
		local idx = mainForm.listVTableFuncs.ItemIndex
		if idx < 0 or idx >= #vtableFunctions then return end
		local vf = vtableFunctions[idx + 1]
		local mv = getMemoryViewForm()
		mv.HexadecimalView.Address = vf.address
		mv.show()
	end
	popupVT.Items.Add(miBrowseHex)

	local miBrowseDisasm = createMenuItem(popupVT)
	miBrowseDisasm.Caption = "Browse in Disassembler"
	miBrowseDisasm.OnClick = function()
		local idx = mainForm.listVTableFuncs.ItemIndex
		if idx < 0 or idx >= #vtableFunctions then return end
		local vf = vtableFunctions[idx + 1]
		local mv = getMemoryViewForm()
		mv.DisassemblerView.SelectedAddress = vf.address
		mv.show()
	end
	popupVT.Items.Add(miBrowseDisasm)

	mainForm.listVTableFuncs.PopupMenu = popupVT

	-- Horizontal splitter between vtable functions and disassembly
	local splitterRightTableFunctions = createSplitter(panelVTable)
	splitterRightTableFunctions.Align = alTop
	splitterRightTableFunctions.Top = panelVTFuncs.Height

	-- --- Disassembly pane (bottom half) ---
	local panelDisasm = createPanel(panelVTable)
	panelDisasm.Align = alClient
	panelDisasm.BevelOuter = bvNone
	panelDisasm.Caption = ""

	local lblDisasm = createLabel(panelDisasm)
	lblDisasm.Align = alTop
	lblDisasm.Height = 20
	lblDisasm.Caption = "  Disassembly:"

	mainForm.memoDisasm = createMemo(panelDisasm)
	mainForm.memoDisasm.Align = alClient
	mainForm.memoDisasm.ReadOnly = true
	mainForm.memoDisasm.WordWrap = false
	mainForm.memoDisasm.ScrollBars = ssBoth
	mainForm.memoDisasm.Font.Name = "Consolas"
	mainForm.memoDisasm.Font.Size = 9

	-- Context menu for disassembly pane
	local popupDisasm = createPopupMenu(mainForm)

	local miCopySelDisasm = createMenuItem(popupDisasm)
	miCopySelDisasm.Caption = "Copy Selected"
	miCopySelDisasm.OnClick = function()
		local text = mainForm.memoDisasm.SelText
		if text and #text > 0 then
			writeToClipboard(text)
			log("Copied selected text to clipboard.")
		end
	end
	popupDisasm.Items.Add(miCopySelDisasm)

	local miCopyDisasm = createMenuItem(popupDisasm)
	miCopyDisasm.Caption = "Copy All"
	miCopyDisasm.OnClick = function()
		local text = mainForm.memoDisasm.Text
		if text and #text > 0 then
			writeToClipboard(text)
			log("Copied disassembly to clipboard.")
		end
	end
	popupDisasm.Items.Add(miCopyDisasm)

	mainForm.memoDisasm.PopupMenu = popupDisasm

	-- ======== Show form and print help ========
	mainForm.show()

	log(SCRIPT_TITLE .. " ready.")
	log("Workflow:")
	log("  1) Select target module (.exe) -> 'Scan for RTTI'")
	log("  2) Filter (Camera, Player, World, Render, etc.)")
	log("  3) Double-click type -> traces TypeDesc -> ObjLocator -> VTable -> Instances")
	log("  4) Double-click instance -> Memory Browser")
	log("  5) 'Dissect Structure' / 'Hex Dump' to inspect layout")
	log("  6) Click an instance -> VTable functions listed in right panel")
	log("  7) Click a vtable function -> Disassembly appears below")
	log("     Double-click a vtable function -> opens CE's disassembler")
	log("")
	log("Reverse lookup:")
	log("  Paste an instance address into 'Instance Lookup' and click 'Identify Type'")
	log("  to walk backward: Instance -> VTable -> ObjLocator -> TypeDesc -> Name")
	log("  Right-click an instance in the list -> 'Identify Type' also works.")
	log("  To reverse lookup a range, specify an instance address as the start address")
	log("  and a range (in hex) to define how big the range should be. Then click 'Identify range'")
	log("")

	btnRefreshMods.OnClick()
end

-- ============================================================================
-- ENTRY POINT
-- ============================================================================
if getOpenedProcessID() == 0 then
	showMessage("Attach Cheat Engine to a process first, then run this script.")
else
	buildUI()
end