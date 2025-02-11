# include "AIPlayer.h"
# include "Parchis.h"

using namespace std;


// VARIABLES
const double masinf = 9999999999.0, menosinf = -9999999999.0;
const double gana = masinf - 1, pierde = menosinf + 1;
const int num_pieces = 3;
const int PROFUNDIDAD_MINIMAX = 4;  // Umbral maximo de profundidad para el metodo MiniMax
const int PROFUNDIDAD_ALFABETA = 6; // Umbral maximo de profundidad para la poda Alfa_Beta


// MÉTODO MOVE (NO MODIFICAR)
bool AIPlayer::move(){

    cout << "Realizo un movimiento automatico" << endl;

    color c_piece;
    int id_piece;
    int dice;
    think(c_piece, id_piece, dice);

    cout << "Movimiento elegido: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    actual->movePiece(c_piece, id_piece, dice);
    return true;
}


// METODO VALORACIONTEST (NO MODIFICAR)
double AIPlayer::ValoracionTest(const Parchis &estado, int jugador)
{
    // Heurística de prueba proporcionada para validar el funcionamiento del algoritmo de búsqueda.

    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador)
    {
        return gana;
    }
    else if (ganador == oponente)
    {
        return pierde;
    }
    else
    {
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;

        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++)
        {
            color c = my_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                // Valoro positivamente que la ficha esté en casilla segura o meta.
                if (estado.isSafePiece(c, j))
                {
                    puntuacion_jugador++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_jugador += 5;
                }
            }
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;

        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++)
        {
            color c = op_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                if (estado.isSafePiece(c, j))
                {
                    // Valoro negativamente que la ficha esté en casilla segura o meta.
                    puntuacion_oponente++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_oponente += 5;
                }
            }
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}


// ***METODO QUE CALCULA LA PUNTUACIÓN POR CADA COLOR
double AIPlayer::Puntos_Color(const Parchis &estado, color c) {

    double puntuacion = 0;
    const int NUM_CASILLAS_TOTALES = 72;

    
    // Fichas en la meta (mayor puntuación cuanto más tenga)
    double fichas_meta = estado.piecesAtGoal(c);
    double meta;

    if (fichas_meta == 0) {
    
        meta = 0;
    
    } else if (fichas_meta == 1) {

        meta = 30;

    } else if (fichas_meta == 2) {

        meta = 40;

    } else {

        meta = 50;
    }


    // Fichas en casa (menor puntuación cuanto más tenga)
    double fichas_casa = estado.piecesAtHome(c);
    double casa;

    if (fichas_casa == 0) {
    
        casa = 0;
    
    } else if (fichas_casa == 1) {

        casa = 20;

    } else if (fichas_casa == 2) {

        casa = 40;

    } else {

        casa = 60;
    }


    // Distancia a meta
    double distancia_meta = 0;

    for(int i = 0; i < 3; i++){ // Tenemos 3 piezas

        distancia_meta += NUM_CASILLAS_TOTALES - estado.distanceToGoal(c,i);
    }


    // Pieza segura
    double pieza_segura = 0;

    for(int i = 0; i < 3; i++){ // Tenemos 3 piezas

        if (estado.isSafePiece(c, i)) {

            pieza_segura += 10;
        }
    }

    // La puntuación corresponde a las que estan en meta + su distancia + seguras - las que estan en casa
    puntuacion = meta + distancia_meta + pieza_segura  - casa ;

    return puntuacion;
}


// ***METODO QUE CALCULA LOS PUNTOS DE CADA JUGADOR CON SU COLOR
double AIPlayer::Puntos_Jugador(const Parchis &estado, int jugador) {

    vector<color> colores;
    vector<double> puntuacion_color(2);
    double puntuacion = 0;

    // Saco los colores del jugador y evaluo su puntación con la función anterior
    colores = estado.getPlayerColors(jugador);
    puntuacion_color[0] = Puntos_Color(estado, colores[0]);
    puntuacion_color[1] = Puntos_Color(estado, colores[1]);


    // Valoro Dados especiales
    for (int j = 0; j < colores.size(); j++) {

        // Movimiento rapido
        if (estado.getPowerBar(j).getPower() >= 0 && estado.getPowerBar(j).getPower() < 50) {

            puntuacion += 7 + (estado.getPowerBar(j).getPower() / 7);
        }

        // Concha roja
        if ((estado.getPowerBar(j).getPower() >= 50 && estado.getPowerBar(j).getPower() < 60) or 
            (estado.getPowerBar(j).getPower() >= 70 && estado.getPowerBar(j).getPower() < 75)) {

            puntuacion += 10;
        }

        // Boom
        if (estado.getPowerBar(j).getPower() >= 60 && estado.getPowerBar(j).getPower() < 65) {

            puntuacion -= 5;
        }

        // Movimiento ultra rápido
        if (estado.getPowerBar(j).getPower() >= 65 && estado.getPowerBar(j).getPower() < 70) {

            puntuacion += 25;
        }

        // Movimiento bala
        if (estado.getPowerBar(j).getPower() >= 75 && estado.getPowerBar(j).getPower() < 80) {

            puntuacion += 40;
        }

        // Catapum
        if (estado.getPowerBar(j).getPower() >= 80 && estado.getPowerBar(j).getPower() < 85) {

            puntuacion -= 15;
        }

        // Concha azul
        if (estado.getPowerBar(j).getPower() >= 85 && estado.getPowerBar(j).getPower() < 90) {

            puntuacion += 50;
        }

        // BoomBoom
        if (estado.getPowerBar(j).getPower() >= 90 && estado.getPowerBar(j).getPower() < 95) {

            puntuacion -= 20;
        }

        // Movimiento Estrella
        if (estado.getPowerBar(j).getPower() >= 95 && estado.getPowerBar(j).getPower() < 100) {

            puntuacion += 60;
        }

        // Catapumchimpum
        if (estado.getPowerBar(j).getPower() >= 100) {

            puntuacion -= 30;
        }
    }


    // Ponderación a cada color (si un color me da mas puntuación que otro lo evaluo mejor)
    if (puntuacion_color[0] > puntuacion_color[1]){
        
        puntuacion += 2.5 * puntuacion_color[0] + 0.5 * puntuacion_color[1];
    
    } else {
        
        puntuacion += 0.5 * puntuacion_color[0] + 2.5 * puntuacion_color[1];
    }

    return puntuacion;
}


// ***MI HEURISTICA 1
double AIPlayer::MiValoracion1(const Parchis &estado, int jugador) {

    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    
    if (ganador == jugador) {
        
        return gana;
    
    } else if (ganador == oponente) {
        
        return pierde;
    
    } else {
        
        double puntos_jugador = Puntos_Jugador(estado, jugador);
        double puntos_oponente = Puntos_Jugador(estado, oponente);


        return puntos_jugador - puntos_oponente;
    }
}


// MI HEURÍSTICA 2
double AIPlayer::MiValoracion2(const Parchis &estado, int jugador) {

    // Heurística 1

    /*
    IDEAS MEJORA HEURÍSTICA

        * DISTANCIA META
        * DADO ESPECIAL
        * FUNCIONES PUNTO 5 DEL GUIÓN 
    */

    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador)
    {
        return gana;
    }
    else if (ganador == oponente)
    {
        return pierde;
    }
    else
    {
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);



        // JUGADOR

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;

        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++)
        {
            color c = my_colors[i];

            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                
                // Valoro positivamente que la ficha esté en casilla segura 
                if (estado.isSafePiece(c, j)) {
                   
                    puntuacion_jugador++;
                } 
                
                 // Valoro positivamente que la ficha esté en meta
                if (estado.getBoard().getPiece(c, j).get_box().type == goal) {
                    
                    puntuacion_jugador += 5;
                }

                // Valoro negativamente si la ficha esta en casa
                if (estado.getBoard().getPiece(c, j).get_box().type == home) {

                    puntuacion_jugador -= 5;
                }

                // Valoro positivamente si la ficha esta en pasillo final
                if (estado.getBoard().getPiece(c, j).get_box().type == final_queue) {

                    puntuacion_jugador += 4;
                }

                // Valoro positivamente si el movimiento fue para comer 
                if (estado.isEatingMove()) {

                    puntuacion_jugador++;
                } 

                // Valoro positivamente si el movimiento fue para llegar a meta
                if (estado.isGoalMove()) {

                    puntuacion_jugador++;
                }

                // Valoro positivamente si el numero de casillas en meta 
                if (estado.piecesAtGoal(c) == 0) {

                    puntuacion_jugador--;
                
                } else if (estado.piecesAtGoal(c) == 1) {

                    puntuacion_jugador += 3;

                } else if (estado.piecesAtGoal(c) == 2) {

                    puntuacion_jugador += 5;

                } else {

                    puntuacion_jugador += 7;

                }

                // Valoro negativamente si el numero de casillas en casa
                if (estado.piecesAtHome(c) == 0) {

                    puntuacion_jugador++;
                
                } else if (estado.piecesAtHome(c) == 1) {

                    puntuacion_jugador -= 3;

                } else if (estado.piecesAtHome(c) == 2) {

                    puntuacion_jugador -= 5;

                } else {

                    puntuacion_jugador -= 7;

                }

                // Fichas Destruidas
                vector<pair<color, int>> destruidas = estado.piecesDestroyedLastMove();
                
                for(int i = 0; i < destruidas.size(); i++){

                    switch(destruidas[i].first){
                        
                        case red:
                        case yellow:

                            if(c == red or c == yellow){
                                
                                puntuacion_jugador -= 20;
                           
                            } else {
                                
                                puntuacion_jugador += 20;
                            }

                            break;
                        
                        case blue:
                        case green:

                            if(c == blue or c == green){
                                
                                puntuacion_jugador -= 20;
                            
                            } else {
                                
                                puntuacion_jugador += 20;
                            }

                            break;
                    }
                }

                // Acciones dado especial
                if (estado.getPowerBar(j).getPower() >= 0 && estado.getPowerBar(j).getPower() < 50) {

                    puntuacion_jugador += 10;
                }

                if ((estado.getPowerBar(j).getPower() >= 50 && estado.getPowerBar(j).getPower() < 60) or 
                    (estado.getPowerBar(j).getPower() >= 70 && estado.getPowerBar(j).getPower() < 75)) {

                    puntuacion_jugador += 12;
                }

                if (estado.getPowerBar(j).getPower() >= 60 && estado.getPowerBar(j).getPower() < 65) {

                    puntuacion_jugador -= 14;
                }

                if (estado.getPowerBar(j).getPower() >= 65 && estado.getPowerBar(j).getPower() < 70) {

                    puntuacion_jugador += 16;
                }

                if (estado.getPowerBar(j).getPower() >= 75 && estado.getPowerBar(j).getPower() < 80) {

                    puntuacion_jugador += 18;
                }

                if (estado.getPowerBar(j).getPower() >= 80 && estado.getPowerBar(j).getPower() < 85) {

                    puntuacion_jugador -= 20;
                }

                if (estado.getPowerBar(j).getPower() >= 85 && estado.getPowerBar(j).getPower() < 90) {

                    puntuacion_jugador += 22;
                }

                if (estado.getPowerBar(j).getPower() >= 90 && estado.getPowerBar(j).getPower() < 95) {

                    puntuacion_jugador -= 24;
                }

                if (estado.getPowerBar(j).getPower() >= 95 && estado.getPowerBar(j).getPower() < 100) {

                    puntuacion_jugador += 26;
                }

                if (estado.getPowerBar(j).getPower() >= 100) {

                    puntuacion_jugador -= 28;
                }

                // Valoro positiva o negativamente el comer fichas
                tuple<color, int, int> last_action = estado.getLastAction();       

                switch(get<0>(last_action)){
                    
                    case red:

                    case yellow:

                        if(estado.eatenPiece().first == blue or estado.eatenPiece().first == green){
                                
                                if (estado.isGoalMove()) {

                                    puntuacion_jugador += 50;

                                } else {

                                    puntuacion_jugador += 20;
                                }
                                
                        } else {
                            
                            if(estado.eatenPiece().first != none){
                                
                                puntuacion_jugador -= 20;
                            
                            } else if (estado.isGoalMove()) {

                                puntuacion_jugador += 40;

                            } else {

                                puntuacion_jugador += 10;
                            }

                        }

                        break;
                    
                    case blue:

                    case green:

                        if(estado.eatenPiece().first == red or estado.eatenPiece().first == yellow){
                                
                                if (estado.isGoalMove()) {

                                    puntuacion_jugador += 50;

                                } else {

                                    puntuacion_jugador += 20;
                                }
                                
                        } else {
                            
                            if(estado.eatenPiece().first != none){
                                
                                puntuacion_jugador -= 20;
                            
                            } else if (estado.isGoalMove()) {

                                puntuacion_jugador += 40;

                            } else {

                                puntuacion_jugador += 10;
                            }

                        }
                        
                        break;

                    case none:
                        break;
                }

                // Distancia
                puntuacion_jugador += 72 - estado.distanceToGoal(c,j);

            }
        }            




        // OPONENTE

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;

        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++)
        {
            color c = op_colors[i];

            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {

                // Valoro negativamente que la ficha esté en casilla segura 
                if (estado.isSafePiece(c, j)) {
                    
                    puntuacion_oponente++;
                } 
                
                // Valoro negativamente que la ficha esté en meta
                if (estado.getBoard().getPiece(c, j).get_box().type == goal) {
                    
                    puntuacion_oponente += 5;
                }

                // Valoro negativamente si la ficha esta en casa
                if (estado.getBoard().getPiece(c, j).get_box().type == home) {

                    puntuacion_oponente -= 5;
                }

                // Valoro positivamente si la ficha esta en pasillo final
                if (estado.getBoard().getPiece(c, j).get_box().type == final_queue) {

                    puntuacion_oponente += 4;
                }

                // Valoro negativamente si el movimiento fue para comer 
                if (estado.isEatingMove()) {

                    puntuacion_oponente++;
                } 

                // Valoro negativamente si el movimiento fue para llegar a meta
                if (estado.isGoalMove()) {

                    puntuacion_oponente++;
                }

               // Valoro positivamente si el numero de casillas en meta 
                if (estado.piecesAtGoal(c) == 0) {

                    puntuacion_oponente--;
                
                } else if (estado.piecesAtGoal(c) == 1) {

                    puntuacion_oponente += 3;

                } else if (estado.piecesAtGoal(c) == 2) {

                    puntuacion_oponente += 5;

                } else {

                    puntuacion_oponente += 7;

                }

                // Valoro negativamente si el numero de casillas en casa
                if (estado.piecesAtHome(c) == 0) {

                    puntuacion_oponente++;
                
                } else if (estado.piecesAtHome(c) == 1) {

                    puntuacion_oponente -= 3;

                } else if (estado.piecesAtHome(c) == 2) {

                    puntuacion_oponente -= 5;

                } else {

                    puntuacion_oponente -= 7;

                }


                // Fichas Destruidas
                vector<pair<color, int>> destruidas = estado.piecesDestroyedLastMove();
                
                for(int i = 0; i < destruidas.size(); i++){

                    switch(destruidas[i].first){
                        
                        case red:
                        case yellow:

                            if(c == red or c == yellow){
                                
                                puntuacion_oponente -= 20;
                           
                            } else {
                                
                                puntuacion_oponente += 20;
                            }

                            break;
                        
                        case blue:
                        case green:

                            if(c == blue or c == green){
                                
                                puntuacion_oponente -= 20;
                            
                            } else {
                                
                                puntuacion_oponente += 20;
                            }

                            break;
                    }
                }

                // Acciones dado especial
                if (estado.getPowerBar(j).getPower() >= 0 && estado.getPowerBar(j).getPower() < 50) {

                    puntuacion_oponente += 10;
                }

                if ((estado.getPowerBar(j).getPower() >= 50 && estado.getPowerBar(j).getPower() < 60) or
                    (estado.getPowerBar(j).getPower() >= 70 && estado.getPowerBar(j).getPower() < 75)) {

                    puntuacion_oponente += 12;
                }

                if (estado.getPowerBar(j).getPower() >= 60 && estado.getPowerBar(j).getPower() < 65) {

                    puntuacion_oponente -= 14;
                }

                if (estado.getPowerBar(j).getPower() >= 65 && estado.getPowerBar(j).getPower() < 70) {

                    puntuacion_oponente += 16;
                }

                if (estado.getPowerBar(j).getPower() >= 75 && estado.getPowerBar(j).getPower() < 80) {

                    puntuacion_oponente += 18;
                }

                if (estado.getPowerBar(j).getPower() >= 80 && estado.getPowerBar(j).getPower() < 85) {

                    puntuacion_oponente -= 20;
                }

                if (estado.getPowerBar(j).getPower() >= 85 && estado.getPowerBar(j).getPower() < 90) {

                    puntuacion_oponente += 22;
                }

                if (estado.getPowerBar(j).getPower() >= 90 && estado.getPowerBar(j).getPower() < 95) {

                    puntuacion_oponente -= 24;
                }

                if (estado.getPowerBar(j).getPower() >= 95 && estado.getPowerBar(j).getPower() < 100) {

                    puntuacion_oponente += 26;
                }

                if (estado.getPowerBar(j).getPower() >= 100) {

                    puntuacion_oponente -= 28;
                }


                // Valoro positiva o negativamente el comer fichas
                tuple<color, int, int> last_action = estado.getLastAction();       

                switch(get<0>(last_action)){
                    
                    case red:

                    case yellow:

                        if(estado.eatenPiece().first == blue or estado.eatenPiece().first == green){
                                
                                if (estado.isGoalMove()) {

                                    puntuacion_oponente += 50;

                                } else {

                                    puntuacion_oponente += 20;
                                }
                                
                        } else {
                            
                            if(estado.eatenPiece().first != none){
                                
                                puntuacion_oponente -= 20;
                            
                            } else if (estado.isGoalMove()) {

                                puntuacion_oponente += 40;

                            } else {

                                puntuacion_oponente += 10;
                            }

                        }

                        break;
                    
                    case blue:

                    case green:

                        if(estado.eatenPiece().first == red or estado.eatenPiece().first == yellow){
                                
                                if (estado.isGoalMove()) {

                                    puntuacion_oponente += 50;

                                } else {

                                    puntuacion_oponente += 20;
                                }
                                
                        } else {
                            
                            if(estado.eatenPiece().first != none){
                                
                                puntuacion_oponente -= 20;
                            
                            } else if (estado.isGoalMove()) {

                                puntuacion_oponente += 40;

                            } else {

                                puntuacion_oponente += 10;
                            }

                        }
                        
                        break;

                    case none:
                        break;
                }

                // Distancia a la meta
                puntuacion_oponente += 72 - estado.distanceToGoal(c,j);
                

            }
        }


        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}


// ALGORITMO DE BÚSQUEDA
double AIPlayer::Poda_AlfaBeta(const Parchis &actual, int jugador, int profundidad, int profundidad_max, color &c_piece, int &id_piece, int &dice, double alpha, double beta, double (*heuristic)(const Parchis &, int)) const {

    // Controlar que pasa si llegamos a la profundidad maxima o ha terminado el juego
    if (profundidad == profundidad_max || actual.gameOver()) {
        
        return heuristic(actual, jugador);
    }

    ParchisBros hijos(actual);
   
    // Sacar jugador en el que estamos
    int currentPlayerId = actual.getCurrentPlayerId();
    bool isMaxNode = (currentPlayerId == jugador);

    // Nodo MAX
    if (isMaxNode) { 

        double maxEval = menosinf;
        
        for (auto it = hijos.begin(); it != hijos.end(); ++it) {
            
            Parchis hijo = *it;
            color temp_c_piece;
            int temp_id_piece;
            int temp_dice;
            
            double eval = Poda_AlfaBeta(hijo, jugador, profundidad + 1, profundidad_max, temp_c_piece, temp_id_piece, temp_dice, alpha, beta, heuristic);
            
            if (eval > maxEval) {
                
                maxEval = eval;
                
                if (profundidad == 0) { // Guardar el mejor movimiento en la profundidad 0
                    
                    c_piece = it.getMovedColor();
                    id_piece = it.getMovedPieceId();
                    dice = it.getMovedDiceValue();
                }
            }

            alpha = max(alpha, eval);
            
            if (beta <= alpha) {
                
                break; // Poda beta
            }
        }

        return maxEval;

    // Nodo MIN
    } else { 

        double minEval = masinf;
        
        for (auto it = hijos.begin(); it != hijos.end(); ++it) {
            
            Parchis hijo = *it;
            color temp_c_piece;
            int temp_id_piece;
            int temp_dice;
            
            double eval = Poda_AlfaBeta(hijo, jugador, profundidad + 1, profundidad_max, temp_c_piece, temp_id_piece, temp_dice, alpha, beta, heuristic);
            
            if (eval < minEval) {
                
                minEval = eval;
            }
            
            beta = min(beta, eval);
            
            if (beta <= alpha) {
                
                break; // Poda alfa
            }
        }

        return minEval;
    }
}


// TUTORIAL --> METODO THINK ALEATORIO
void AIPlayer::thinkAleatorio(color & c_piece, int & id_piece, int & dice) const{
    
    /*

    // OBJETIVO: Asignar a las variables c_piece, id_piece, dice (pasadas por referencia) los valores,respectivamente, de:
        
        // - color de ficha a mover: yellow, blue, red, green y none
        // - identificador de la ficha que se va a mover: 0, 1, 2 y SKIP_TURN
        // - valor del dado con el que se va a mover la ficha: 1, 2, 4, 5, 6, 10, 20 y power (dado especial)

    // TAREAS:
        
        // 1. Algoritmo de búsqueda
        // 2. Heurística

    // CONSIDERACIONES GUION

        - En primer lugar, va a ser necesario realizar una búsqueda en profundidad partiendo del estado actual del juego. En
        consecuencia, para cada nodo se hace necesario obtener todos los posibles tableros a los que se
        podría pasar, con cada uno de los posibles movimientos. Para ello, se ha implementado la clase
        ParchisBros que se encarga de la generación de los nodos hijos posibles a partir de un estado
        actual del parchis. Esta clase incluye un iterador que se usará para ir recorriendo los nodos hijos
        del nodo actual. Dado que desde un punto de vista estratégico, en muchas ocasiones un valor de
        dado mayor puede suponer una mayor ventaja, el operador++ del iterador se ha implementado
        recorriendo los hijos en orden descendente del dado.

        - En segundo lugar, es importante destacar que en el juego propuesto un turno se corresponde
        con un único movimiento de ficha, independientemente de que ese movimiento sea repetido
        por el mismo jugador tras sacar un 6, que sea un movimiento de contarse 10 o 20 tras llegar a la
        meta o comer, o que sea la acción que se ejecute mediante un dado especial. En consecuencia,
        como sucesor de un nodo MÁX podríamos encontrarnos de nuevo otro nodo MÁX (lo mismo
        para los MÍN). Por tanto debemos tener en cuenta que la secuencia de nodos no va a ir
        alternándose necesariamente en cada nivel. Tras sacar un 6 como nodo MÁX bajaríamos a un
        nuevo nodo MÁX, con 1 más de profundidad. Igualmente, tras comer ficha bajaríamos a un
        nuevo nodo del mismo tipo, en el que únicamente tendremos que elegir de entre nuestras
        posibles fichas con cuál contarnos 20. En cualquier caso, en todo momento podremos saber si
        somos un nodo MÁX o MIN, ya que conocemos el jugador que llama a la heurística y las
        funciones como getCurrentPlayerId, de la clase Parchis, nos indican a qué jugador le toca
        mover en cada turno. Recordemos que un nodo debería ser MÁX cuando el jugador que mueve
        es el que llamó al algoritmo de búsqueda.

        - En tercer lugar, puede que el algoritmo sea muy lento inicialmente por los dados pero luego al quedarle menos y al tener menos opciones por mover irá pensando más rápido.

        - En cuarto lugar, mirar bien las clases y métodos de la sección 5 del guión.
        
    */


    // El id de mi jugador actual.
    int player = actual->getCurrentPlayerId();

    // Vector que almacenará los dados que se pueden usar para el movimiento
    vector<int> current_dices;

    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<tuple<color, int>> current_pieces;

    // Se obtiene el vector de dados que se pueden usar para el movimiento
    current_dices = actual->getAvailableNormalDices(player);

    // Elijo un dado de forma aleatoria.
    dice = current_dices[rand() % current_dices.size()];

    // Se obtiene el vector de fichas que se pueden mover para el dado elegido
    current_pieces = actual->getAvailablePieces(player, dice);

    // Si tengo fichas para el dado elegido muevo una al azar.
    if (current_pieces.size() > 0)
    {
        int random_id = rand() % current_pieces.size();
        id_piece = get<1>(current_pieces[random_id]); // get<i>(tuple<...>) me devuelve el i-ésimo
        c_piece = get<0>(current_pieces[random_id]);  // elemento de la tupla
    }
    else
    {
        // Si no tengo fichas para el dado elegido, pasa turno (la macro SKIP_TURN me permite no mover).
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    }

    /*
    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda
    double alpha = menosinf, beta = masinf; // Cotas iniciales de la poda AlfaBeta

    // Llamada a la función para la poda
    valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
    
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    // Si quiero poder manejar varias heurísticas, puedo usar la variable id del agente para usar una u otra.
    switch(id){

        case 0: // Asociar aqui valoracionTest
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
            break;

        case 1: // Asociar aqui la mejor heuristica
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiValoracion1);
            break;

        case 2:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiValoracion2);
            break;
    }
    
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;
    */
}


// TUTORIAL --> METODO THINK ALEATORIO MAS INTELIGENTE
void AIPlayer::thinkAleatorioMasInteligente(color &c_piece, int &id_piece, int &dice) const
{
    // El número de mi jugador actual.
    int player = actual->getCurrentPlayerId();

    // Vector que almacenará los dados que se pueden usar para el movimiento.
    vector<int> current_dices;

    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<tuple<color, int>> current_pieces;

    // Obtengo el vector de dados que puedo usar para el movimiento.
    // - actual->getAvailableNormalDices(player) -> solo dados disponibles en ese turno.
    // Importante : "getAvailableNormalDices" me da los dados que puedo usar en el turno actual.
    // Por ejemplo, si me tengo que contar 10 o 20 solo me saldrán los dados 10 y 20.
    // Puedo saber qué más dados tengo, aunque no los pueda usar en este turno, con:
    // - actual->getNormalDices(player) -> todos los dados
    current_dices = actual->getAvailableNormalDices(player);

    // En vez de elegir un dado al azar, miro primero cuáles tienen fichas que se puedan mover.
    vector<int> current_dices_que_pueden_mover_ficha;

    for (int i = 0; i < current_dices.size(); i++)
    {
        // Se obtiene el vector de fichas que se pueden mover para el dado elegido.
        current_pieces = actual->getAvailablePieces(player, current_dices[i]);
        
        // Si se pueden mover fichas para el dado actual, lo añado al vector de dados que pueden mover fichas.
        if (current_pieces.size() > 0)
        {
            current_dices_que_pueden_mover_ficha.push_back(current_dices[i]);
        }
    }

    // Si no tengo ningún dado que pueda mover fichas, paso turno con un dado al azar (la macro SKIP_TURN me permite no mover).
    if (current_dices_que_pueden_mover_ficha.size() == 0)
    {
        dice = current_dices[rand() % current_dices.size()];
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    }

    // En caso contrario, elijo un dado de forma aleatoria de entre los que pueden mover ficha.
    else
    {
        dice = current_dices_que_pueden_mover_ficha[rand() % current_dices_que_pueden_mover_ficha.size()];
        
        // Se obtiene el vector de fichas que se pueden mover para el dado elegido.
        current_pieces = actual->getAvailablePieces(player, dice);
       
       // Muevo una ficha al azar de entre las que se pueden mover.
        int random_id = rand() % current_pieces.size();
        id_piece = get<1>(current_pieces[random_id]);
        c_piece = get<0>(current_pieces[random_id]);
    }
}


// TUTORIAL --> METODO THINK FICHA MAS ADELANTADA
void AIPlayer::thinkFichaMasAdelantada(color &c_piece, int &id_piece, int &dice) const
{
    // Elijo el dado haciendo lo mismo que el jugador anterior.
    thinkAleatorioMasInteligente(c_piece, id_piece, dice);
    
    // Tras llamar a esta función, ya tengo en dice el número de dado que quiero usar.
    // Ahora, en vez de mover una ficha al azar, voy a mover (o a aplicar
    // el dado especial a) la que esté más adelantada
    // (equivalentemente, la más cercana a la meta).
    int player = actual->getCurrentPlayerId();
    
    vector<tuple<color, int>> current_pieces = actual->getAvailablePieces(player, dice);
    
    int id_ficha_mas_adelantada = -1;
    color col_ficha_mas_adelantada = none;
    int min_distancia_meta = 9999;

    for (int i = 0; i < current_pieces.size(); i++)
    {
        // distanceToGoal(color, id) devuelve la distancia a la meta de la ficha [id] del color que le indique.
        color col = get<0>(current_pieces[i]);
        int id = get<1>(current_pieces[i]);
        int distancia_meta = actual->distanceToGoal(col, id);
       
        if (distancia_meta < min_distancia_meta)
        {
            min_distancia_meta = distancia_meta;
            id_ficha_mas_adelantada = id;
            col_ficha_mas_adelantada = col;
        }
    }

    // Si no he encontrado ninguna ficha, paso turno.
    if (id_ficha_mas_adelantada == -1)
    {
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    }

    // En caso contrario, moveré la ficha más adelantada.
    else
    {
        id_piece = id_ficha_mas_adelantada;
        c_piece = col_ficha_mas_adelantada;
    }
}


// TUTORIAL --> METODO THINK MEJOR OPCION
void AIPlayer::thinkMejorOpcion(color &c_piece, int &id_piece, int &dice) const
{
    // Vamos a mirar todos los posibles movimientos del jugador actual accediendo a los hijos del estado actual.
    // Cuando ya he recorrido todos los hijos, la función devuelve el estado actual. De esta forma puedo saber cuándo paro de iterar.
    // Para ello, vamos a iterar sobre los hijos con la función de Parchis getChildren().
    // Esta función devuelve un objeto de la clase ParchisBros, que es una estructura iterable sobre la que se pueden recorrer todos los hijos del estado sobre el que se llama.
    
    ParchisBros hijos = actual->getChildren();
    
    bool me_quedo_con_esta_accion = false;
    
    // La clase ParchisBros viene con un iterador muy útil y sencillo de usar.
    // Al hacer begin() accedemos al primer hijo de la rama, y cada vez que hagamos ++it saltaremos al siguiente hijo.
    // Comparando con el iterador end() podemos consultar cuándo hemos terminado de visitar los hijos.
    // Voy a moverme a la casilla siempre con la que gane más energía, salvo que me encuentre con
    // algún movimiento muy interesante, como comer fichas o llegar a la meta.
    
    int current_power = actual->getPowerBar(this->jugador).getPower();
    
    int max_power = -101; // Máxima ganancia de energía.
    
    for (ParchisBros::Iterator it = hijos.begin(); it != hijos.end() and !me_quedo_con_esta_accion; ++it)
    {
        Parchis siguiente_hijo = *it;
        
        // Si en el movimiento elegido comiera ficha, llegara a la meta o ganara, me quedo con esa acción.
        // Termino el bucle en este caso.
        
        if (siguiente_hijo.isEatingMove() or (siguiente_hijo.isGoalMove()) or (siguiente_hijo.gameOver() and siguiente_hijo.getWinner() == this->jugador))
        {
            me_quedo_con_esta_accion = true;
            c_piece = it.getMovedColor();
            id_piece = it.getMovedPieceId();
            dice = it.getMovedDiceValue();
        }

        // En caso contrario, me voy quedando con el que me dé más energía.
        else{
            
            int new_power = siguiente_hijo.getPower(this->jugador);
            
            if (new_power - current_power > max_power){
               
                c_piece = it.getMovedColor();
                id_piece = it.getMovedPieceId();
                dice = it.getMovedDiceValue();
                max_power = new_power - current_power;
            }
        }
    }

// Si he encontrado una acción que me interesa, la guardo en las variables pasadas por referencia.
// (Ya lo he hecho antes, cuando les he asignado los valores con el iterador).

}


// THINK
void AIPlayer::think(color & c_piece, int & id_piece, int & dice) const{
    
    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda
    double alpha = menosinf, beta = masinf; // Cotas iniciales de la poda AlfaBeta

    // Llamada a la función para la poda
    valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
    
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    // Si quiero poder manejar varias heurísticas, puedo usar la variable id del agente para usar una u otra.
    switch(id){

        case 0: // Asociar aqui valoracionTest
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
            break;

        case 1: // Asociar aqui la mejor heuristica
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiValoracion1);
            break;

        case 2: 
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiValoracion2);
            break;

        case 3:
            thinkAleatorio(c_piece, id_piece, dice);
            break;
        
        case 4:
            thinkAleatorioMasInteligente(c_piece, id_piece, dice);
            break;

        case 5:
            thinkFichaMasAdelantada(c_piece, id_piece, dice);
            break;

        case 6:
            thinkMejorOpcion(c_piece, id_piece, dice);
            break;
    }
    
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;
    
}