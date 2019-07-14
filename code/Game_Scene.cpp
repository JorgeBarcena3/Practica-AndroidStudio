/*
 * GAME SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#include "Game_Scene.hpp"
#include "Final_Scene.hpp"
#include "Menu_Scene.hpp"

#include <cstdlib>
#include <basics/Canvas>
#include <basics/Director>

using namespace basics;
using namespace std;

namespace project_template
{
    // ---------------------------------------------------------------------------------------------
    // ID y ruta de las texturas que se deben cargar para esta escena.

    Game_Scene::Texture_Data Game_Scene::textures_data[] =
    {

        { ID(pausa),         "game-scene/pausa-button.png"},
        { ID(clicable),      "game-scene/click.png"},
        { ID(background),    "game-scene/background.png"},
        { ID(reiniciar_btn), "menu/restartbtn.png"},
        { ID(menu_btn),      "menu/menubtn.png"}

    };

    // Para determinar el número de items en el array textures_data, se divide el tamaño en bytes
    // del array completo entre el tamaño en bytes de un item:

    unsigned Game_Scene::textures_count = sizeof(textures_data) / sizeof(Texture_Data);

    // ---------------------------------------------------------------------------------------------
    // Definiciones de los atributos estáticos de la clase:


    constexpr int Game_Scene:: max_time;
    int  Game_Scene:: clicks = 0;


    // ---------------------------------------------------------------------------------------------

    Game_Scene::Game_Scene()
    {
        // Se establece la resolución virtual (independiente de la resolución virtual del dispositivo).
        // En este caso no se hace ajuste de aspect ratio, por lo que puede haber distorsión cuando
        // el aspect ratio real de la pantalla del dispositivo es distinto.

        canvas_width  =  720;
        canvas_height =  1280;

        aspect_ratio_adjusted = false;
        game_paused = false;

        timer_in_pause = 0;


        // Se inicia la semilla del generador de números aleatorios:
        srand (unsigned(time(nullptr)));

        // Se inicializan otros atributos:

        initialize ();
    }

    // ---------------------------------------------------------------------------------------------
    // Algunos atributos se inicializan en este método en lugar de hacerlo en el constructor porque
    // este método puede ser llamado más veces para restablecer el estado de la escena y el constructor
    // solo se invoca una vez.

    bool Game_Scene::initialize ()
    {
        state     = LOADING;
        suspended = true;
        gameplay  = UNINITIALIZED;

        clicks = 0;

        return true;
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::suspend ()
    {
        suspended = true;               // Se marca que la escena ha pasado a primer plano
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::resume ()
    {
        suspended = false;              // Se marca que la escena ha pasado a segundo plano
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::handle (Event & event)
    {
        if (state == RUNNING)               // Se descartan los eventos cuando la escena está LOADING
        {
            if (gameplay == WAITING_TO_START)
            {
                start_playing ();           // Se empieza a jugar cuando el usuario toca la pantalla
                                            // por primera vez

                timer.reset();
            }
            else switch (event.id)
            {
                case ID(touch-started):
                {
                    //Guardamos la posición tocada por el usuario
                    Point2f touch_location = {
                            *event[ID(x)].as< var::Float > (),
                            *event[ID(y)].as< var::Float > ()
                    };

                    if(game_paused)
                    {
                        if(menuBtn->contains(touch_location))
                            director.run_scene (shared_ptr< Scene >(new Menu_Scene()));
                        else if(reiniciarBtn->contains(touch_location)) {
                            director.run_scene (shared_ptr< Scene >(new Game_Scene()));

                        }
                        else {
                            //Reiniciamos el juego
                            pause_the_game(false);
                            //Hacer la pausa
                            timer_in_pause = timer.get_elapsed_seconds() - timer_in_pause;
                            //Mostramos los botones
                            menuBtn->hide();
                            reiniciarBtn->hide();
                            clicable_ptr->show();
                        }
                    }
                    else
                    {
                        if(clicable_ptr->contains(touch_location)) {
                            int randX = rand() % ((int)(canvas_width * .9) - (int)(canvas_width * 0.1) + 1) + (int)(canvas_width * 0.1);
                            int randY = rand() % ((int)(canvas_height * 0.9f) - (int)(canvas_height * 0.1) + 1) + (canvas_height * 0.1);


                            //RANDOM float
                            float random = ((float) rand()) / (float) RAND_MAX;
                            float diff = 0.3 - 0.2;
                            float r = 0.3 + (random * diff);

                            clicable_ptr->set_position({randX, randY});
                            clicable_ptr->set_scale(r);
                            clicks++;
                        } else
                        if(pause->contains(touch_location)){

                            //Pausamos el juego
                            pause_the_game(true);
                            //Hacer la pausa
                            timer_in_pause = timer.get_elapsed_seconds();
                            //Mostramos los botones del menu de pausa
                            menuBtn->show();
                            reiniciarBtn->show();
                            clicable_ptr->hide();
                        }
                    }

                    break;
                }
                case ID(touch-moved):
                {
                    break;
                }

                case ID(touch-ended):       // El usuario deja de tocar la pantalla
                {

                    break;
                }
            }
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::update (float time)
    {
        if (!suspended) switch (state)
        {
            case LOADING: load_textures  ();     break;
            case RUNNING: run_simulation (time); break;
            case ERROR:   break;
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::render (Context & context)
    {
        if (!suspended)
        {
            // El canvas se puede haber creado previamente, en cuyo caso solo hay que pedirlo:

            Canvas * canvas = context->get_renderer< Canvas > (ID(canvas));

            // Si no se ha creado previamente, hay que crearlo una vez:

            if (!canvas)
            {
                 canvas = Canvas::create (ID(canvas), context, {{ canvas_width, canvas_height }});
            }

            // Si el canvas se ha podido obtener o crear, se puede dibujar con él:

            if (canvas)
            {
                canvas->clear ();

                switch (state)
                {
                    case LOADING: ; break;
                    case RUNNING: render_playfield (*canvas); break;
                    case ERROR:   break;
                }
            }
        }
    }

    // ---------------------------------------------------------------------------------------------
    // En este método solo se carga una textura por fotograma para poder pausar la carga si el
    // juego pasa a segundo plano inesperadamente. Otro aspecto interesante es que la carga no
    // comienza hasta que la escena se inicia para así tener la posibilidad de mostrar al usuario
    // que la carga está en curso en lugar de tener una pantalla en negro que no responde durante
    // un tiempo.

    void Game_Scene::load_textures ()
    {
        if (textures.size () < textures_count)          // Si quedan texturas por cargar...
        {
            // Las texturas se cargan y se suben al contexto gráfico, por lo que es necesario disponer
            // de uno:

            Graphics_Context::Accessor context = director.lock_graphics_context ();

            if (context)
            {
                // Se ajusta el aspect ratio si este no se ha ajustado
                if(!aspect_ratio_adjusted){

                    adjust_aspect_ratio(context);
                }

                // Se carga la siguiente textura (textures.size() indica cuántas llevamos cargadas):

                Texture_Data   & texture_data = textures_data[textures.size ()];
                Texture_Handle & texture      = textures[texture_data.id] = Texture_2D::create (texture_data.id, context, texture_data.path);

                // Se comprueba si la textura se ha podido cargar correctamente:

                if (texture) context->add (texture); else state = ERROR;

               //Incluimos la fuente
                font.reset (new Raster_Font("game-scene/fonts/impact.fnt", context));

            }
        }
        else {

            create_gameobjects();
            restart_game();

            state = RUNNING;
        }

    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::create_gameobjects()
    {

        //GameObject_Handle  nombre_objeto(new GameObject (textures[ID(nombre_ID)].get() ));
        //...

        // 2) Se establecen los anchor y position de los GameObject
        // Si no se especifica se colocan por defecto en la posición (0,0) y el anchor en el centro
        //nombre_objeto->set_anchor (...);
        //nombre_objeto->set_position ({coordenada_x, coordenada_y});

        // 3) Se añaden a la lista de game objects
        //gameobjects.push_back (nombre_objeto);


        // Se crean los objetos no dinámicos de la escena
        GameObject_Handle clicable  (new GameObject (textures[ID(clicable)].get(), real_aspect_ratio, 0.5));
        GameObject_Handle background (new GameObject (textures[ID(background)].get(), 2));
        GameObject_Handle pausa_button (new GameObject (textures[ID(pausa)].get(), real_aspect_ratio, 0.5));
        GameObject_Handle reiniciar_btn(new GameObject (textures[ID(reiniciar_btn)]. get(), real_aspect_ratio, 2));
        GameObject_Handle menu_btn(new GameObject (textures[ID(menu_btn)]. get(), real_aspect_ratio, 2));


        pausa_button->set_position({pausa_button -> get_width() * 0.5f + (pausa_button -> get_width()), (canvas_height - pausa_button -> get_height())});
        background->set_position({(canvas_width * 0.5f), (canvas_height * 0.5f)});
        background->set_sullScreen();
        clicable->set_position({(canvas_width * 0.5f), (canvas_height - (clicable  -> get_height() * 0.5f))});
        reiniciar_btn-> set_position({(canvas_width * 0.5f), (canvas_height * 0.5f)});
        menu_btn-> set_position({(canvas_width * 0.5f), ((reiniciar_btn -> get_bottom_y()) - 10 - (menu_btn -> get_height() * 0.5f))});


//Determina el Z-Index
        gameobjects.push_back(background);
        gameobjects.push_back(clicable);
        gameobjects.push_back(pausa_button);
        gameobjects.push_back(reiniciar_btn);
        gameobjects.push_back(menu_btn);

        // Se guardan punteros a los gameobjects que se van a usar frecuentemente:
        // nombre_puntero = nombre_objeto.get();

        clicable_ptr = clicable.get();
        pause = pausa_button.get();
        menuBtn = menu_btn.get();
        reiniciarBtn = reiniciar_btn.get();

        //Ocultamos los botones del menu de pausa
        menuBtn->hide();
        reiniciarBtn->hide();

    }

    // ---------------------------------------------------------------------------------------------
    // Cuando el juego se inicia por primera vez o cuando se reinicia porque un jugador pierde, se
    // llama a este método para restablecer los gameobjects:

    void Game_Scene::restart_game()
    {
        //nombre_objeto->nombre_metodo(parametros);

        clicks = 0;
        pause -> show();

        gameplay = WAITING_TO_START;
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::start_playing ()
    {
        gameplay = PLAYING;
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::run_simulation (float time)
    {

        if(gameplay != ENDING) {

            // Se actualiza el estado de todos los gameobjects:
            for (auto &gameobject : gameobjects) {
                gameobject->update(time);
            }

            //Manejamos el timer
            if(!game_paused){

                currentTime = timer.get_elapsed_seconds() - timer_in_pause;
            }

            // Comprobación de tiempo restante de la partida:
            if (currentTime >= max_time) {
                gameplay = ENDING;
            }


        }
        else
        {

            director.run_scene (shared_ptr< Scene >(new Final_Scene(clicks)));
        }


    }


    // ---------------------------------------------------------------------------------------------


    // ---------------------------------------------------------------------------------------------
    // Simplemente se dibujan todos los gameobjects que conforman la escena.

    void Game_Scene::render_playfield (Canvas & canvas)
    {
        for (auto & gameobject : gameobjects)
        {
            gameobject->render (canvas);
        }

        if(font && !game_paused && gameplay == PLAYING)
            drawFont(canvas);
    }

    // ---------------------------------------------------------------------------------------------
    // Ajusta el aspect ratio

    void Game_Scene::adjust_aspect_ratio(Context & context)
    {

        real_aspect_ratio = float( context->get_surface_width () ) / context->get_surface_height ();

        // Si se desea mantener el ancho y ajustar el alto (disposición vertical)
        canvas_height = unsigned ( canvas_width * real_aspect_ratio);

        aspect_ratio_adjusted = true;
    }

    // ---------------------------------------------------------------------------------------------
    // Pausa el juego

    void Game_Scene::pause_the_game(bool paused) {

        game_paused = paused;
     }

    void Game_Scene::drawFont(Canvas & canvas) {

        //Determinamos el color
        canvas.set_color (1, 1, 0);

        if(!game_paused) {
            // Se crea el text layout a partir de la cadena creada:
            Text_Layout puntuacion_text(*font, std::to_wstring(clicks));

            // Y se dibuja en el centro de la pantalla:
            canvas.draw_text({canvas_width * 0.5, canvas_height * 0.1}, puntuacion_text, CENTER);


            //Tiempo que nos queda
            // Se crea el text layout a partir de la cadena creada:
            int tiempo = floor(max_time - currentTime);
            Text_Layout tiempo_restante(*font, std::to_wstring(tiempo));

            // Y se dibuja en el centro de la pantalla:
            canvas.draw_text({canvas_width * 0.8, canvas_height * 0.9}, tiempo_restante, RIGHT);
        } else {
            //Texto del menu de pause
            std::string s = "El juego esta pausado, haz click en cualquier lugar de la pantalla para continuar";
            std::wstring ws(s.begin(), s.end());
            Text_Layout texto_pausa(*font, ws);

            // Y se dibuja en el centro de la pantalla:
            canvas.draw_text({canvas_width * 0.5, canvas_height * 0.5}, texto_pausa, CENTER);
        }

    }


}


