namespace IGCSClient.Classes
{
	public struct AspectRatio
	{
		public AspectRatio(int horizontal, int vertical)
		{
			this.Horizontal = horizontal;
			this.Vertical = vertical;
			this.Description = string.Empty;
		}

		/// <summary>
		/// Factor used for calculating the height using a width
		/// </summary>
		public decimal ARFactorHeight
		{
			get { return (decimal)this.Vertical / (decimal)this.Horizontal; }
		}
		
		
		/// <summary>
		/// Factor used for calculating the width using a height
		/// </summary>
		public decimal ARFactorWidth
		{
			get { return (decimal)this.Horizontal / (decimal)this.Vertical; }
		}


		public override string ToString()
		{
			return this.ToString(true);
		}


		public string ToString(bool appendDescription)
		{
			string suffix = string.IsNullOrWhiteSpace(this.Description) || !appendDescription ? string.Empty : string.Format(" ({0})", this.Description);
			return string.Format("{0}:{1}{2}", this.Horizontal, this.Vertical, suffix);
		}


		// these are generated. Description isn't taken into account.
		public override bool Equals(object obj)
		{
			return obj is AspectRatio ratio &&
				   this.Horizontal == ratio.Horizontal &&
				   this.Vertical == ratio.Vertical;
		}

		// these are generated
		public override int GetHashCode()
		{
			var hashCode = 1211898032;
			hashCode = hashCode * -1521134295 + this.Horizontal.GetHashCode();
			hashCode = hashCode * -1521134295 + this.Vertical.GetHashCode();
			return hashCode;
		}

		public int Horizontal { get; }
		public int Vertical { get; }
		public string Description { get; set; }
	}
}