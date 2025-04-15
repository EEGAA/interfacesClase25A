/*
    La busqueda se realiza girando de 0 a 180 grados con pasos de 2 y registrando
    en un float vector[90] el valor medido por el sensor ultrasonico para cada
    paso del barrido

    Despues con un metodo de ordenamiento, o busqueda se discrimina la menor o mayor lectura
    segun sea la preferencia.

    El proposito no solo es saber que medida es la menor o mayor, sino saber a que angulo
    corresponde
*/
class dtaDIS{
    private:
        float dta[181], x = 181;
    public:
        dtaDIS(){
            for(int i = 0; i < x; i++)
                this->dta[i] = 0;
        }
        void setDTA(int p, float x){
            this->dta[p] = x;
        }
        float getDTA(int p){
            return this->dta[p];
        }
        int busqueda(int bnd){
            int posicion = 0;
            float posible = this->dta[0];
            int aux = 0, i;
            for(i = 1; i < x; i++ ){
                if((bnd == 0 && this->dta[i] < posible) || 
                   (bnd == 1 && this->dta[i] > posible)){
                    
                    posible = this->dta[i];
                    posicion = i;
                }
            }
            if(this->dta[posicion+1] == this->dta[posicion]){//Si es posible que sea una secuencia
                for(i = posicion+1; i < x; i++){
                    if(this->dta[i] == this->dta[posicion])
                        aux++;
                    else
                        break;
                }
                return posicion + aux/2;
            }
                return posicion;
        }
    };