using System.ComponentModel.DataAnnotations;

namespace RickMortyApp
{
	public class Characters
	{
		[Key]
		public int id { get; set; }

		public string name { get; set; }
		public string status { get; set; }
		public string species { get; set; }
		public string nameoforigin { get; set; }
		public string image { get; set; }
	}
}
