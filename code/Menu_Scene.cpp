/*
 * MENU SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#include "Menu_Scene.hpp"
#include "Game_Scene.hpp"

#include <basics/Canvas>
#include <basics/Director>
#include <basics/Transformation>

using namespace basics;
using namespace std;

namespace project_template
{

    // ---------------------------------------------------------------------------------------------
    // ID y ruta de las texturas que se deben cargar para esta escena.

    Menu_Scene::Texture_Data Menu_Scene::textures_data[] =
            {
                    { ID(play_button_id),          "menu/comenzarbtn.png"},
                    { ID(background_id),           "menu/background.png"},
                    { ID(instructions_button_id),  "menu/objetivobtn.png"},
                    { ID(logo_button_id),          "menu/logo.png"},
                    { ID(objetivo_text_id),          "menu/instruccionesinfo.png"}

            };

    // Para determinar el número de items en el array textures_data, se divide el tamaño en bytes
    // del array completo entre el tamaño en bytes de un item:

    unsigned Menu_Scene::textures_count = sizeof(textures_data) / sizeof(Texture_Data);

    Menu_Scene::Menu_Scene()
    {
        state         = LOADING;
        suspended     = true;
        canvas_width  = 720;
        canvas_height = 1280;

        aspect_ratio_adjusted = false;
    }

    // ---------------------------------------------------------------------------------------------

    bool Menu_Scene::initialize ()
    {
        state     = LOADING;
        suspended = true;
        showing_instructions = false;

        return true;
    }

    // ---------------------------------------------------------------------------------------------

    void Menu_Scene::handle (basics::Event & event)
    {
        if (state == READY)// Se descartan los eventos cuando la escena está LOADING
        {
            switch (event.id)
            {
                case ID(touch-started):// El usuario toca la pantalla
                {
                    Point2f touch_location = { *event[ID(x)].as< var::Float > (), *event[ID(y)].as< var::Float > () };


                    if(!showing_instructions)
                    {

                        if      (play_button_pointer->contains(touch_location)) play();
                        else if (instructions_button_pointer->contains(touch_location)) show_instructions(true);

                    }

                    else if(showing_instructions)
                    {
                        show_instructions(false);

                    }

                    break;

                }
                case ID(touch-moved):
                {
                    break;
                }

                case ID(touch-ended):
                {

                    break;
                }
            }
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Menu_Scene::update (float time)
    {

        if (!suspended) switch (state)
            {
                case LOADING: load_textures  ();     break;
                case READY: run_simulation (time); break;
                case ERROR:   break;
            }

    }

    // ---------------------------------------------------------------------------------------------

    void Menu_Scene::render (Graphics_Context::Accessor & context)
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
                    case LOADING: break;
                    case READY:   render_playfield (*canvas); break;
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

    void Menu_Scene::load_textures ()
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

//Se cargan las tecturas
                Texture_Data   & texture_data = textures_data[textures.size ()];
                Texture_Handle & texture      = textures[texture_data.id] = Texture_2D::create (texture_data.id, context, texture_data.path);


                if (texture) context->add (texture); else state = ERROR;


            }
        }
        else {

            create_gameobjects();
            state = READY;
        }
    }


    // ---------------------------------------------------------------------------------------------
    // Ajusta el aspect ratio

    void Menu_Scene::adjust_aspect_ratio(Context & context)
    {

        real_aspect_ratio = float( context->get_surface_width () ) / context->get_surface_height ();

        // Si se desea mantener el ancho y ajustar el alto (disposición vertical)
        canvas_height = unsigned ( canvas_width * real_aspect_ratio);

        aspect_ratio_adjusted = true;
    }


    // ---------------------------------------------------------------------------------------------
    // Crea los objetos de la escena

    void Menu_Scene::create_gameobjects()
    {


        // Se crean los objetos no dinámicos de la escena
        GameObject_Handle play_button_object(new GameObject (textures[ID(play_button_id)]. get(), real_aspect_ratio, 2));
        GameObject_Handle instructions_button_object(new GameObject (textures[ID(instructions_button_id)]. get(), real_aspect_ratio, 2));
        GameObject_Handle logo_object(new GameObject (textures[ID(logo_button_id)].get(), real_aspect_ratio, 0.5));
        GameObject_Handle background(new GameObject (textures[ID(background_id)].get(), 2));
        GameObject_Handle instruccionesInfo(new GameObject (textures[ID(objetivo_text_id)].get(), real_aspect_ratio, 1.25f));

        //Posicionamos los objetos
        logo_object-> set_position({(canvas_width * 0.5f), (canvas_height - (logo_object  -> get_height() * 0.5f))});
        play_button_object-> set_position({(canvas_width * 0.5f), (canvas_height * 0.5f)});
        instructions_button_object-> set_position({(canvas_width * 0.5f), ((play_button_object -> get_bottom_y()) - 10 - (instructions_button_object -> get_height() * 0.5f))});
        background-> set_position({(canvas_width * 0.5f), (canvas_height * 0.5f)});
        background->set_sullScreen();
        instruccionesInfo-> set_position({(canvas_width * 0.5f), (canvas_height * 0.5f)});

		//Gamoebject que se van a usar
        objects.push_back(background);
        objects.push_back(play_button_object);
        objects.push_back(logo_object);
        objects.push_back(instructions_button_object);
        objects.push_back(instruccionesInfo);

        //Inizialiamos los punteros
        play_button_pointer         = play_button_object .get();
        instructions_button_pointer = instructions_button_object.get();
        logo_pointer                = logo_object.get();
        instructions_text_pointer = instruccionesInfo.get();

        //Ocultamos las instrucciones
        instructions_text_pointer->hide();


    }

    // ---------------------------------------------------------------------------------------------
    void Menu_Scene::run_simulation (float time)
    {
        // Se actualiza el estado de todos los gameobjects:

        for (auto & obj : objects)
        {
            obj->update (time);
        }


    }

    // ---------------------------------------------------------------------------------------------
    // Simplemente se dibujan todos los gameobjects que conforman la escena.

    void Menu_Scene::render_playfield (Canvas & canvas)
    {
        for (auto & obj : objects)
        {
            obj->render (canvas);
        }
    }



    // ---------------------------------------------------------------------------------------------
    // Se inicia la partida

    void Menu_Scene::play() {
        director.run_scene (shared_ptr<Scene>(new Game_Scene));
    }

    // ---------------------------------------------------------------------------------------------
    // Se muestran las instrucciones

    void Menu_Scene::show_instructions(bool _showing) {

        if(!_showing)
        {
            showing_instructions = false;
            logo_pointer                -> show();
            instructions_button_pointer -> show();
            play_button_pointer         -> show();
            instructions_text_pointer->hide();

        }
        else if(_showing)
        {
            showing_instructions = true;

            instructions_text_pointer->show();

            logo_pointer                -> hide();
            instructions_button_pointer -> hide();
            play_button_pointer         -> hide();

        }



    }

}
