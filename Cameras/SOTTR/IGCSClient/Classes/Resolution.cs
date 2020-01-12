namespace IGCSClient.Classes
{
	/// <summary>
	/// Simple struct to use for a resolution.
	/// </summary>
	public struct Resolution
	{
		public Resolution(AspectRatio ar, int horizontalResolution, int verticalResolution, string description)
		{
			this.AR = ar;
			this.HorizontalResolution = horizontalResolution;
			this.VerticalResolution = verticalResolution;
			this.Description = description;
		}

		// these are generated
		public override bool Equals(object obj)
		{
			return obj is Resolution resolution &&
				   this.AR.Equals(resolution.AR) &&
				   this.HorizontalResolution == resolution.HorizontalResolution &&
				   this.VerticalResolution == resolution.VerticalResolution;
		}

		// these are generated
		public override int GetHashCode()
		{
			var hashCode = 1445329852;
			hashCode = hashCode * -1521134295 + this.AR.GetHashCode();
			hashCode = hashCode * -1521134295 + this.HorizontalResolution.GetHashCode();
			hashCode = hashCode * -1521134295 + this.VerticalResolution.GetHashCode();
			return hashCode;
		}

		public AspectRatio AR { get;  }
		public int HorizontalResolution { get; }
		public int VerticalResolution { get; }
		public string Description { get; }
	}
}