using Microsoft.EntityFrameworkCore;

namespace RickMortyApp
{
	public class AppDBContext : DbContext
	{
		public AppDBContext(DbContextOptions<AppDBContext> options) : base(options) { }
	}
}
