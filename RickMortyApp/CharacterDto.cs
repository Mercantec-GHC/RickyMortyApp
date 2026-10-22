using System.ComponentModel.DataAnnotations;

namespace RickMortyApp
{
	public class CharacterDto
	{
		[Required]
		public string? characterName { get; set; } = null;
		public string? characterSpecies { get; set; }
		public string? characterStatus { get; set; }
		public string? characterOrigin { get; set; }
		public string? characterImageUrl { get; set; }

		public Characters ToModel()
		{
			return new Characters
			{
				name = characterName ?? string.Empty,
				species = characterSpecies ?? string.Empty,
				status = characterStatus ?? string.Empty,
				nameoforigin = characterOrigin ?? string.Empty,
				image = characterImageUrl ?? string.Empty
			};
		}

	}
}
