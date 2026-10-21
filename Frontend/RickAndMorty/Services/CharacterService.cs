
using Npgsql;
using RickAndMorty.Models;

namespace RickAndMorty.Services
{
    public class CharacterService
    {
        private readonly string _connectionString;

        public CharacterService(IConfiguration configuration)
        {
            _connectionString =
                configuration.GetConnectionString("NeonDb")
                ?? throw new InvalidOperationException(
                    "NeonDb connection string is missing.");
        }

        public async Task<List<Character>> GetCharactersAsync()
        {
            var characters = new List<Character>();

            await using var connection =
                new NpgsqlConnection(_connectionString);

            await connection.OpenAsync();

            const string sql = """
                SELECT id, name, status, species, nameoforigin, image
                FROM public."Characters"
                ORDER BY id;
                """;

            await using var command =
                new NpgsqlCommand(sql, connection);

            await using var reader =
                await command.ExecuteReaderAsync();

            while (await reader.ReadAsync())
            {
                var character = new Character
                {
                    Id = reader.GetInt32(reader.GetOrdinal("id")),
                    Name = reader["name"] as string ?? string.Empty,
                    Status = reader["status"] as string ?? string.Empty,
                    Species = reader["species"] as string ?? string.Empty,
                    Nameoforigin = reader["nameoforigin"] as string ?? string.Empty,
                    Image = reader["image"] as string ?? string.Empty
                };

                characters.Add(character);
            }

            return characters;
        }
    }
}
