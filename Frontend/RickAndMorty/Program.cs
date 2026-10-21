
using RickAndMorty.Components;
using RickAndMorty.Services;

namespace RickAndMorty
{
    public class Program
    {
        public static void Main(string[] args)
        {
            var builder = WebApplication.CreateBuilder(args);

            // Add Blazor services
            builder.Services.AddRazorComponents()
                .AddInteractiveServerComponents();

            // Register our database service
            builder.Services.AddScoped<CharacterService>();

            var app = builder.Build();

            // Configure the HTTP request pipeline
            if (!app.Environment.IsDevelopment())
            {
                app.UseExceptionHandler("/Error");
                app.UseHsts();
            }

            app.UseHttpsRedirection();

            app.UseStaticFiles();
            app.UseAntiforgery();

            app.MapRazorComponents<App>()
                .AddInteractiveServerRenderMode();

            app.Run();
        }
    }
}
