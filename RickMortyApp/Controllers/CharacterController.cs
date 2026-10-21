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

		[HttpPost]
		public async Task<IActionResult> CreateCharacter(Characters character)
		{
			_context.Characters.Add(character);
			await _context.SaveChangesAsync();
			return CreatedAtAction(nameof(GetAllCharacters), new { id = character.id }, character);
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

