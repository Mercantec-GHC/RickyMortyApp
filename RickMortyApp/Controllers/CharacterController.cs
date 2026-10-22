using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;

namespace RickMortyApp.Controllers
{

	[ApiController]
	[Route("api/[controller]")]
	public class CharacterController : ControllerBase
	{
		private readonly AppDBContext _context;
		public CharacterController(AppDBContext context)
		{
			_context = context;
		}

		[HttpGet]
		public async Task<List<Characters>> GetAllCharacters()
		{
			return await _context.Characters.ToListAsync();
		}

		[HttpGet("{id}")]
		public async Task<IActionResult> GetCharacterById(int id)
		{
			var character = await _context.Characters.FindAsync(id);
			if (character == null)
			{
				return NotFound();
			}
			return Ok(character);
		}

		[HttpPost]
		public async Task<IActionResult> CreateCharacter([FromBody] CharacterDto character)
		{
			if (string.IsNullOrWhiteSpace(character.characterName) )
			{
				return BadRequest("Character name is required.");
			}

			var model = character.ToModel();

			_context.Characters.Add(model);
			await _context.SaveChangesAsync();

			return Ok(model);
		}

		[HttpDelete]
		public async Task<IActionResult> DeleteCharacter(int id)
		{
			var character = await _context.Characters.FindAsync(id);
			if (character == null)
			{
				return NotFound();
			}
			_context.Characters.Remove(character);
			await _context.SaveChangesAsync();
			return NoContent();
		}
	}
}

