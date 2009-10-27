#include "controller.hpp"
#include "editor.hpp"
#include "main.hpp"
#include "video.hpp"
#include "volume.hpp"

using namespace std;

map<Block::types, Sprite> Editor::block_sprites;

Editor::Editor( void )
{
	if (block_sprites.empty())
	{
		block_sprites = Block(0, 0, Block::none).sprites;
		block_sprites[Block::ball] = Sprite("sprites/editor/ball.ppm");
		block_sprites[Block::exit] = Sprite("sprites/editor/exit.ppm");
	}
}

bool Editor::load( const string &level_data_path )
{
	bool temp = level.load(level_data_path);
	reset();
	
	return temp;
}

void Editor::reset( void )
{
	x_offset = 0;
	y_offset = 0;
	
	cursor_x = 0;
	cursor_y = 0;
}

void Editor::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	for (vector<Block>::const_iterator block = level.blocks.begin(); block != level.blocks.end(); ++block)
		block_sprites[block->type].blit(surface, x_offset + block->x, y_offset + block->y, alpha);
}

void editor_loop( SDL_Surface *surface, Editor &editor )
{
	Fader fader;
	fader.fade(Fader::in);
	
	bool quit = false, done = false;
	
	while (!done && !global_quit)
	{
		SDL_WaitEvent(NULL);
		
		int updates = 0, frames = 0;
		
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				global_quit = true;
				break;
				
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym)
				{
				case SDLK_RETURN:
					cerr << "TODO: editor menu" << endl; // TODO
					break;
				case SDLK_ESCAPE:
					quit = true;
					break;
					
				case SDLK_PLUS:
					trigger_volume_change(0.1f);
					break;
				case SDLK_MINUS:
					trigger_volume_change(-0.1f);
					break;
					
				default:
					break;
				}
				break;
				
			case SDL_USEREVENT:
				switch (e.user.code)
				{
				case USER_UPDATE:
					++updates;
					break;
				case USER_FRAME:
					++frames;
					break;
				}
				break;
				
			default:
				break;
			}
		}
		
		while (updates--)
		{
			// update controller
			for (vector<Controller *>::iterator c = controllers.begin(); c != controllers.end(); ++c)
				(*c)->update();
			
			if (quit)
				fader.fade(Fader::out);
				
			fader.update();
			done = fader.is_done() && fader.was_fading(Fader::out);
			
			update_volume_notification();
		}
		
		if (frames--)
		{
			editor.draw(surface, fader.value());
			
			draw_volume_notification(surface);
			
			SDL_Flip(surface);
			
			if (frames > 0)
				clog << "dropped " << frames << " frame(s)" << endl;
		}
	}
}
