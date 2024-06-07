#ifndef __AI_PLAYER_H__
#define __AI_PLAYER_H__

# include "Attributes.h"
# include "Player.h"

// CLASE AIPLAYER
class AIPlayer: public Player{
    

    protected:
        
        //Id identificador del jugador. Se puede usar dentro del think para seleccionar heurísticas
        const int id;
   
   
    public:
        

        /**
         * @brief Constructor de un objeto AIPlayer
         * 
         * @param name Nombre del jugador
         */
        inline AIPlayer(const string & name):Player(name), id(1){};
        

        /**
         * @brief Constructor de un objeto AIPlayer 
         * 
         * @param name Nombre  del jugador
         * @param id Id del jugador
         */
        inline AIPlayer(const string & name, const int id):Player(name), id(id){};


        /**  
         * @brief Función que percibe el parchís y al jugador actual.
         * Asigna el tablero en actual y el id del jugador.
         * 
         * @param p Instancia Parchis que se quiere percibir
         */
        inline virtual void perceive(Parchis &p){Player::perceive(p);}


        /** 
         * @brief Función abstracta que define el movimiento devuelto por el jugador.
         * Llama a la función movePiece con el valor asignado a los parámetros pasados 
         * por referencia.
         * 
         * @return true
         * @return false 
         */
        virtual bool move();
        

        /**  
         * @brief Función que se encarga de decidir el mejor movimiento posible a 
         * partir del estado actual del tablero. Asigna a las variables pasadas por
         * referencia el valor de color de ficha, id de ficha y dado del mejor movimiento.
         * 
         * @param c_piece Color de la ficha
         * @param id_piece Id de la ficha
         * @param dice Número de dado
         */
        virtual void think(color & c_piece,  int & id_piece, int & dice) const;


        // MÉTODOS TUTORIAL

        /** 
         * @brief Función que se encarga de implementar la misma funcionalidad que el think.
         * 
         * @param c_piece Color de la ficha
         * @param id_piece Id de la ficha
         * @param dice Número de dado
         */
        void thinkAleatorio(color &c_piece, int &id_piece, int &dice) const;


        /** 
         * @brief Función que se encarga de buscar para qué dados del jugador se puede mover al 
         * menos una ficha. Elegimos aletoriamente para qué dados del jugador se puede mover al
         * menos una ficha. Solo pasaremos turno cuando no se pueda mover ninguna ficha para 
         * ninguno de los dados disponibles.
         * 
         * @param c_piece Color de la ficha
         * @param id_piece Id de la ficha
         * @param dice Número de dado
         */
        void thinkAleatorioMasInteligente(color &c_piece, int &id_piece, int &dice) const;


        /**
         * @brief Función que se encarga de mover la ficha más adelantada, teniendo en cuenta la
         * distancia entre casillas con el objetivo de llevarla antes a la meta. Se pasa turno si
         * no hubiera ninguna ficha que tenga esta opción.
         * 
         * @param c_piece Color de la ficha
         * @param id_piece Id de la ficha
         * @param dice Número de dado
         */
        void thinkFichaMasAdelantada(color &c_piece, int &id_piece, int &dice) const;


        /** 
         * @brief Función que se encarga de decidir el mejor movimiento posible en función de si 
         * la acción a realizar es comer, llegar a meta o ganar la partida. Si no se cumple alguna
         * de esa condición, me quedo con la que mayor valor de energía obtenga.
         * 
         * @param c_piece Color de la ficha
         * @param id_piece Id de la ficha
         * @param dice Número de dado
         */
        void thinkMejorOpcion(color &c_piece, int &id_piece, int &dice) const;


        /**
         * @brief Método que determina si el player es inteligente (decide el mejor movimiento)
         * o no. True para AIPlayer.
         * 
         * @return true 
         * @return false 
         */
        inline virtual bool canThink() const{return true;}


        /**
         * @brief Heurística de prueba para validar el algoritmo de búsqueda
         * 
         * @param estado Instancia de Parchis con el estado actual de la partida.
         * @param jugador Id del jugador actual (0 o 1)
         * @return double 
         */
        static double ValoracionTest(const Parchis &estado, int jugador);

        
        /**
         * @brief Método que calcula la puntuación de cada color
         * 
         * @param estado Instancia de Parchis con el estado actual de la partida.
         * @param jugador Id del jugador actual (0 o 1)
         * @return double 
         */
        static double Puntos_Color(const Parchis &estado, color c);


        /**
         * @brief Método que calcula la puntuación de cada jugador
         * 
         * @param estado Instancia de Parchis con el estado actual de la partida.
         * @param jugador Id del jugador actual (0 o 1)
         * @return double 
         */
        static double Puntos_Jugador(const Parchis &estado, int jugador);


        /**
         * @brief 1 Heurística 
         * 
         * @param estado Instancia de Parchis con el estado actual de la partida.
         * @param jugador Id del jugador actual (0 o 1)
         * @return double 
         */
        static double MiValoracion1(const Parchis &estado, int jugador);


        /**
         * @brief 2 Heurística 
         * 
         * @param estado Instancia de Parchis con el estado actual de la partida.
         * @param jugador Id del jugador actual (0 o 1)
         * @return double 
         */
        static double MiValoracion2(const Parchis &estado, int jugador);


        /**
         * @brief Método de la poda alfa-beta
         * 
         * @param actual Instancia constante y por referencia de la clase Parchis
         * @param jugador Identificador del jugador
         * @param profundidad Nivel de la profundidad
         * @param profundidad_max Nivel de profundidad máxima
         * @param c_piece Color de la ficha
         * @param id_piece Identificador de la ficha
         * @param dice Numero de dado
         * @param alpha Elemento fundamental para la poda empleado en nodos max
         * @param beta Elemento fundamental para la poda empleado en nodos min
         * @param heuristic Una funcion que toma una referencia constante de Parchis y un entero y devuelve un double
         * @return double
         */
        double Poda_AlfaBeta(const Parchis &actual, int jugador, int profundidad, int profundidad_max, color &c_piece, int &id_piece, int &dice, double alpha, double beta, double (*heuristic)(const Parchis &, int)) const;
};
#endif