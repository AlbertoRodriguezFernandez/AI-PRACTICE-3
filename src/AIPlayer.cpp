# include "AIPlayer.h"
# include "Parchis.h"


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


// ---TODO
void AIPlayer::think(color & c_piece, int & id_piece, int & dice) const{
    
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

    // TUTORIAL
    switch(id){

        case 0:
            thinkAleatorio(c_piece, id_piece, dice);
            break;
        
        case 1:
            thinkAleatorioMasInteligente(c_piece, id_piece, dice);
            break;

        case 2:
            //thinkFichaMasAdelantada(c_piece, id_piece, dice);
            break;

        case 3:
            //thinkMejorOpcion(c_piece, id_piece, dice);
            break;
    }


    /* GUIÓN

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


