# Rick and Morty App

A  project with an ASP.NET Core API, a Blazor frontend,
and an Arduino client.

## Technologies

- ASP.NET Core Web API
- Entity Framework Core
- PostgreSQL hosted on Neon
- Blazor
- Arduino MKR WiFi 1010 (2)
- Docker and Vercel

## Features

- Retrieve characters from the database
- Create characters through the API
- Display characters in the frontend
- Check database connectivity
- Delete characters on the arduino

## API endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | /api/Character | Get all characters |
| GET | /api/Character/{id} | Get a character by ID |
| POST | /api/Character | Create a character |
| DELETE | /api/Character/{id} | Delete a character by ID |
| GET | /api/Status/dbhealthcheck | Check database connectivity |

### Create a character

POST /api/Character

Example request body:

{
  "characterName": "Rick Sanchez",
  "characterSpecies": "Human",
  "characterStatus": "Alive",
  "characterOrigin": "Earth",
  "characterImageUrl": "https://example.com/rick.png"
}

## Local setup

1. Clone the repository.

2. Start the API:

   dotnet run --project <path-to-api.csproj>

3. Start the Blazor frontend (frontend folder):

   dotnet run --project <path-to-frontend.csproj>

Configure the frontend and Arduino to use the API address.

## Deployment

The API and frontend are deployed as separate Vercel projects.
Each project uses its own Dockerfile.vercel and vercel.json.

Do not commit database credentials to the repository.
