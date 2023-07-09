#ifndef MATRIX_L
#define MATRIX_L

#include <vector>

using namespace std;

template <class V>
struct matriz {

	matriz(int x_, int y_) {
		valores.resize(x_);
		for (int i = 0; i < valores.size(); valores[i].resize(y_), i++);
		x = x_; y = y_;
	}
	matriz(vector<vector<V>> valores_) {
		valores = valores_;
		x = valores.size();
		y = valores[0].size();
	}
  void Transponer(){
    for(int i = 0; i < x; i ++)
      for(int j = i; j < y; j++)
        swap(valores[i][j],valores[j][i]);
  }
  void Invertir(){
    V det = Determinante();
    valores = Adjunta();
    for(int i = 0; i < x; i++)
      for(int j = 0; j < y; j++)
        valores[i][j] = valores[i][j]/det;
  }
  V Determinante(){
    vector<vector<V>> aux = valores;
    float det = 1.0;
    for (int i = 0; i < x; i++) {
        int maxRow = i;
        for (int j = i + 1; j < y; j++) 
            if (fabs(valores[j][i]) > fabs(valores[maxRow][i]))
                maxRow = j;
        if (maxRow != i) {
            for (int k = 0; k < x; k++) 
                std::swap(valores[maxRow][k], valores[i][k]);
            det *= -1.0;
        }
        for (int j = i + 1; j < x; j++) {
            float factor = valores[j][i] / valores[i][i];
			if(valores[i][i]==0) factor = 0;
            for (int k = i; k < x; k++)
                valores[j][k] -= factor * valores[i][k];
        }
        det *= valores[i][i];
    }
    valores = aux;
    return det;
  }
  matriz<V> subMatriz(int fila, int columna){
    matriz<V> resultado(x-1,y-1);
    int r = 0;
    for (int i = 0; i < x; i++) {
      if (i == fila) continue;
      int c = 0;
      for (int j = 0; j < y; j++) {
        if (j == columna) continue;
        resultado[r][c] = valores[i][j];
        c++;
      }
      r++;
    }
    return resultado;
  }
  vector<vector<V>> Adjunta(){
    matriz<V> resultado(x,y);
    
    for(int i = 0; i < x; i++)
      for(int j = 0; j < y; j++){
        matriz<V> submatriz(subMatriz(i,j));
        resultado[i][j] = pow(-1, i + j) * submatriz.Determinante();
      }
    resultado.Transponer();
    return resultado.valores;
  }
  vector<V>& operator [](int p1){ return valores[p1]; }
  matriz<V>& operator *(const matriz<V>& p2){
	matriz<V>* resultado = new matriz<V> (x, y);
	for (int i = 0; i < x; i++) 
		for (int j = 0; j < y; j++) {
			(resultado->valores)[i][j] = 0;
			for (int k = 0; k < y; k++)
				(resultado->valores)[i][j] += valores[i][k] * p2.valores[k][j];
		}
	return *resultado;
  }
  void operator *=(const matriz<V>& p2){(*this) = (*this) * p2;}
  friend ostream& operator<<(ostream& os, matriz<V>& dt){
  for(int i = 0; i < dt.x; i++){
    for(int j = 0; j < dt.y; j++)
      os << dt[i][j] << " ";
    os << "\n";
  }
  return os;
}

	vector<vector<V>> valores;
	int x, y;

};


template<class V>
vector<vector<V>> Matriz_T(V Tx, V Ty, V Tz = 0){
  return vector<vector<V>>{
    {1,0,0,Tx},
    {0,1,0,Ty},
    {0,0,1,Tz},
    {0,0,0,1}
  };
}
template<class V>
vector<vector<V>> Matriz_S(V Sx, V Sy, V Sz = 1){
  return vector<vector<V>>{
    {Sx,0,0,0},
    {0,Sy,0,0},
    {0,0,Sz,0},
    {0,0,0,1}
  };
}
template<class V>
vector<vector<V>> Matriz_R2(V angulo){
  V a = cos(angulo*M_PI/180),
  b = sin(angulo*M_PI/180);
  return vector<vector<V>>{
    {a,-b,0,0},
    {b, a,0,0},
    {0, 0,1,0},
    {0, 0,0,1}
  };
}
template<class V>
vector<vector<V>> Matriz_R3(V x, V y, V z){
  V cx = cos(x*M_PI/180),
  sx = sin(x*M_PI/180),
  cy = cos(y*M_PI/180),
  sy = sin(y*M_PI/180),
  cz = cos(z*M_PI/180),
  sz = sin(z*M_PI/180);
  return vector<vector<V>>{
    { cx*cy*cz-sx*sz, cx*cy*sz+sx*sz,-cx*sy,0},
    {-sx*cy*cz-cx*sz,-sx*cy*sz+cx*sz, sx*sy,0},
    {          sy*cz,          sy*sz,    cy,0},
    {              0,              0,     0,1}
  };
}
template<class V>
vector<vector<V>> Matriz_R3_X(V angulo){
  V a = cos(angulo*M_PI/180),
  b = sin(angulo*M_PI/180);
  return vector<vector<V>>{
    {1,0, 0,0},
    {0,a,-b,0},
    {0,b, a,0},
    {0,0, 0,1}
  };
}
template<class V>
vector<vector<V>> Matriz_R3_Y(V angulo){
  V a = cos(angulo*M_PI/180),
  b = sin(angulo*M_PI/180);
  return vector<vector<V>>{
    { a,0,b,0},
    { 0,1,0,0},
    {-b,0,a,0},
    { 0,0,0,1}
  };
}
template<class V>
vector<vector<V>> Matriz_R3_Z(V angulo){
  V a = cos(angulo*M_PI/180),
  b = sin(angulo*M_PI/180);
  return vector<vector<V>>{
    { a,-b,0,0},
    { b,a,0,0},
    { 0,0,1,0},
    { 0,0,0,1}
  };
}

template<class V>
struct vec{
  vec(V* a, int tam){
    val = new V[dim = tam];
    for(int i = 0; i < tam; i++) val[i] = a[i];
  }
  vec(vector<V> valores){
    int tam = valores.size();
    val = new V[dim = tam];
    for(int i = 0; i < tam; i++) val[i] = valores[i];
  }
  
V get_modulo(){
    modulo = 0;
    for(int i = 0; i < dim; i++)
      modulo += val[i] * val[i];
    modulo = sqrt(modulo);
    return modulo;
  }
  vec<V>* normalizar(){
    get_modulo();
    V* aux = new V[dim];
    for(int i = 0; i < dim; i++)
      aux[i] = val[i]/modulo;
    vec<V>* rpta = new vec<V>(aux,dim);
    return rpta;
  }

  vec<V>* get_normal(vec<V> b){
    V* aux = new V[dim];
    int i,j,k;
    for(i = 0; i < dim; i++){
      j = (i+1)%dim;
      k = (i+2)%dim;
      aux[j] = val[k]*b[i] - val[i]*b[k];
    }
    vec<V>* rpta = new vec<V>(aux,dim);
    return rpta;
  }
  float get_angulo(vec<V> b){
    get_modulo();
    b.get_modulo();
    return acos((*this)*b/(b.modulo*modulo))*180/M_PI;
  }
  
  V* val;
  int dim;
  double modulo;

  V operator [](int p1){ return p1<dim ? val[p1] : 0; }
  V operator *(const vec<V>& p1){
	V result = 0;
	for(int i = 0; i < dim; i++)
		result += this[i]*p1[i];
	return result;
  }
	vec<V> operator -(const vec<V>& p1) const {
		vec<V> aux(dim);
		for (int i = 0; i < dim; i++)
			aux[i] = (*this)[i] - p1[i];
		return aux;
	}
};
float* Transformar_Vertices(float* Vertex, int puntos, vector<vector<float>> T){
	
	// Pasamos las posiciones a una matriz 4x4 sin colores
	vector<vector<float>> M_V_aux(puntos);
	for(int i = 0; i < puntos ; i++){
		for(int j = 0; j < 3 ; j++)
			M_V_aux[i].push_back(Vertex[i*11+j]);
		M_V_aux[i].push_back(1.0);
	}
	
	vec<float> aux1(M_V_aux[0]);
	vec<float> aux2(M_V_aux[1]);
	vec<float> aux3(M_V_aux[2]);
	
	vector<float> aux4 = aux1-aux2;
	
	// Multiplicamos la matriz 4x4 de puntos por la matriz de transformación
	matriz<float> M_V(M_V_aux);
	matriz<float> M_T(T);
	M_T.Transponer();
	M_V = M_V*M_T;
	
	// Volvemos a poner los colores
	for(int i = 0; i < puntos; i++){
		for(int j = 0; j < 3; j++)
			Vertex[i*11+j] = M_V[i][j];
	}
	for(int i = 0; i < 6; i++){
	}
	return Vertex;
}
float* Transformar_Normal(float* Vertex, int puntos, vector<vector<float>> T){
	
	// Pasamos las posiciones a una matriz 4x4 sin colores
	vector<vector<float>> M_V_aux(puntos);
	
	for(int i = 0; i < puntos ; i++){
		for(int j = 3; j < 6 ; j++)
			M_V_aux[i].push_back(Vertex[i*11+j]);
		M_V_aux[i].push_back(1.0);
	}
	// Multiplicamos la matriz 4x4 de puntos por la matriz de transformación
	matriz<float> M_V(M_V_aux);
	matriz<float> M_T(T);
	M_T.Invertir(); 
	M_V = M_V*M_T;
	// Volvemos a poner los colores
	for(int i = 0; i < puntos; i++)
		for(int j = 3; j < 6; j++)
			Vertex[i*11+j] = M_V[i][j-3];
	return Vertex;
}
float* Destransformar_Vertices(float* Vertex, int puntos, vector<vector<float>> T){
	
	// Pasamos las posiciones a una matriz 4x4 sin colores
	vector<vector<float>> M_V_aux(puntos);
	for(int i = 0; i < puntos ; i++){
		for(int j = 0; j < 3 ; j++)
			M_V_aux[i].push_back(Vertex[i*11+j]);
		M_V_aux[i].push_back(1.0);
	}
	
	// Multiplicamos la matriz 4x4 de puntos por la matriz de transformación
	matriz<float> M_V(M_V_aux);
	matriz<float> M_T(T);
	M_T.Transponer();				// Usamos la matriz de la transformada para dejar los puntos intactos
	M_T.Invertir();
	M_V = M_V*M_T;
	// Volvemos a poner los colores
	for(int i = 0; i < puntos; i++)
		for(int j = 0; j < 3; j++)
			Vertex[i*11+j] = M_V[i][j];
		
	return Vertex;
}
float* Destransformar_Normal(float* Vertex, int puntos, vector<vector<float>> T){
	
	// Pasamos las posiciones a una matriz 4x4 sin colores
	vector<vector<float>> M_V_aux(puntos);
	for(int i = 0; i < puntos ; i++){
		for(int j = 3; j < 6 ; j++)
			M_V_aux[i].push_back(Vertex[i*11+j]);
		M_V_aux[i].push_back(1.0);
	}
	
	// Multiplicamos la matriz 4x4 de puntos por la matriz de transformación
	matriz<float> M_V(M_V_aux);
	matriz<float> M_T(T);
	//M_T.Transponer();				// Usamos la matriz de la transformada para dejar los puntos intactos
	M_V = M_V*M_T;
	// Volvemos a poner los colores
	for(int i = 0; i < puntos; i++)
		for(int j = 3; j < 6; j++)
			Vertex[i*11+j] = M_V[i][j-3];
		
	return Vertex;
}
void Set_Color(float* Vertex, int tam, float R, float G, float B, int init = 0){
	for(int i = init; i < tam ; i++){
		Vertex[i*8+3]=R/255;
		Vertex[i*8+4]=G/255;
		Vertex[i*8+5]=B/255;
	}
}

#endif