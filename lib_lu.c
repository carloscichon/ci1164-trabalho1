#include "lib_lu.h"
#include <likwid.h>
#include <likwid-marker.h>

/*!
  \brief Encontra o pivo para o pivoteamento parcial.
  \param A Matriz a ser encontrado o pivo.
  \param i Número da coluna que está sendo testada.
  \param n Tamanho da matriz nxn.
  \return Retorna o maior valor abaixo de i.
*/
int encontraMax(double **A, int i, int n){
  double numLinha=0;
  int maior=-1;
  for (int j = i; j < n; j++){
    if (A[j][i] >= numLinha){
      maior = j;
      numLinha = A[j][i];
    }
  }

  if(maior == -1)
    return i;

  return maior;
}

/*!
  \brief Copia um vetor em outro.
  \param a Vetor fonte.
  \param b Vetor destino.
  \param n Tamanho do vetor.
*/
void copyV(double *a, double *b, int n){
  for (int i = 0; i < n; i++)
    b[i] = a[i];
}

/*!
  \brief Printa um vetor (debug)
  \param v Vetor a ser printado
  \param n Tamanho do vetor
*/
void printVetor(double *v, int n, FILE *saida){
  for (int i = 0; i < n; i++){
    fprintf(saida, "%1.23e ", v[i]);
  }
  fprintf(saida, "\n");
}

/*!
  \brief Preenche com 1s a matriz identidade
  \param matriz Matriz identidade
  \param n Tamanho da matriz identidade
*/
void preencheIdent(double **matriz, int n){
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      if(i == j)
        matriz[i][j] = 1.0;
}

/*!
  \brief Copia um vetor para uma coluna de matriz
  \param m Matriz destino
  \param v Vetor a ser copiado
  \param n Tamanho da matriz e do vetor
  \param i Numero da coluna a ser copiada
*/
void cpVetorEmMatriz(double **m, double *v, int n, int i){
  for (int j = 0; j < n; j++)
      m[j][i] = v[j];
}

/*!
  \brief Copia uma coluna de matriz para um vetor
  \param m Matriz origem
  \param v Vetor destino
  \param n Tamanho da matriz e do vetor
  \param i Numero da coluna a ser copiada
*/
void cpMatrizEmVetor(double **m, double *v, int n, int i){
  for (int j = 0; j < n; j++)
      v[j] = m[j][i];
}

/*!
  \brief Copia uma matriz de doubles em outra
  \param a Matriz origem
  \param b Matriz destino
  \param n Tamanho das matrizes quadradas
*/
void copiaMatriz(double **a, double **b, unsigned int n){
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      b[i][j] = a[i][j];
}

/*!
  \brief Troca duas linhas de uma matriz.
  \param matriz Matriz a ter linhas trocadas.
  \param n Tamango da matriz nxn.
  \param i Número da linha a ser trocada.
  \param iPivo Número da outra linha a ser trocada.
*/
void trocaLinha(double **matriz, int n, int i, int iPivo){
  double *auxVet = malloc(n * sizeof(double));
  if (auxVet == NULL){
    fprintf(stderr, "Erro de alocação\n");
    exit(4);
  }
  
  //double aux;
  
  copyV(matriz[i], auxVet, n);
  copyV(matriz[iPivo], matriz[i], n);
  copyV(auxVet, matriz[iPivo], n);
  free(auxVet);

}

/*!
  \brief Troca duas linhas de um Sistema Triangular
  \param L Sistema triangular a ter linhas trocadas
  \param i Número da linha a ser trocada.
  \param iPivo Número da outra linha a ser trocada.
*/
void trocaLinhaL(S_tri *L, int i, int iPivo){
    if(i > 0) {
        double temp;
        for(int j = 0; j <= i - 1; j++) {
            temp = L->coef[i][j];
            L->coef[i][j] = L->coef[iPivo][j];
            L->coef[iPivo][j] = temp;
        }
    }
}

void trocaPosVetor(double *v, int i, int iPivo){
  double aux = v[i];
  v[i] = v[iPivo];
  v[iPivo] = aux;
}

/*!
  \brief Realiza a triangularização de uma matriz e salva as operações num Sistema Triangular.
  \param entrada Matriz a ter linhas trocadas.
  \param n Tamanho da matriz nxn.
  \param L Sistema Triangular que receberá as operações (L da fatoração LU).
  \param pivo Flag de pivoteamento parcial
*/
int triangulariza(double **entrada, int n, S_tri *L, int pivo, double *func){
  // modificada em 09/08 para o trab 2
  int iPivo;
    for (int i = 0; i < n; i++){
      iPivo = 0;
        if (pivo){
            iPivo = encontraMax(entrada, i, n);
            if(iPivo != i){
                trocaLinha(entrada, n, i, iPivo);
                trocaPosVetor(func, i, iPivo);
                trocaLinhaL(L, i, iPivo);
            }
        }
        for (int k=i+1; k < n; k++){
            if(entrada[i][i] == 0.0){
              fprintf(stderr, "Divisão por 0 encontrada.\n");
              exit(1);
            }
            double m = entrada[k][i] / entrada[i][i];
            L->coef[k][i] = m;
            entrada[k][i] = 0.0;
            for (int j = i+1; j < n; j++)
                entrada[k][j] -= entrada[i][j] * m;

        }
    }
    //printMatriz(entrada, n, n, stdout);

}

/*!
  \brief Realiza a triangularização de uma matriz e salva as operações num Sistema Triangular.
  \param entrada Matriz a ter linhas trocadas.
  \param n Tamanho da matriz nxn.
  \param L Sistema Triangular que receberá as operações (L da fatoração LU).
  \param pivo Flag de pivoteamento parcial
*/
int triangulariza_otimiz(double **entrada, int n, S_tri *L, int pivo, double *func){
  int iPivo;
  unsigned int m = 2; //stride

    for (int i = 0; i < n; i++){
      iPivo = 0;
        if (pivo){
            iPivo = encontraMax(entrada, i, n);
            if(iPivo != i){
                trocaLinha(entrada, n, i, iPivo);
                trocaPosVetor(func, i, iPivo);
                trocaLinhaL(L, i, iPivo);
            }
        }
        for (int k=i+1; k < n-(n%m)-1; k+=m){
            if(entrada[i][i] == 0.0){
              fprintf(stderr, "Divisão por 0 encontrada.\n");
              exit(1);
            }
            printf("Ola1\n");

            double m = entrada[k][i] / entrada[i][i];
            printf("Ola2 k+1=%d \n", k+1);
            double m1 = entrada[k+1][i] / entrada[i][i];
            L->coef[k][i] = m;
            L->coef[k+1][i] = m1;
            entrada[k][i] = 0.0;
            entrada[k+1][i] = 0.0;
            for (int j = i+1; j < n; j++){
                entrada[k][j] -= entrada[i][j] * m;
                entrada[k+1][j] -= entrada[i][j] * m1;
            }
            printf("Ola3\n");
        }
        printf("Ola4\n");
        for (int k=n-(n%m)-1; k < n; k++){
            if(k!=i){
              if(entrada[i][i] == 0.0){
                fprintf(stderr, "Divisão por 0 encontrada.\n");
                exit(1);
              }
              printf("Ola5\n");
              double mr = entrada[k][i] / entrada[i][i];
              L->coef[k][i] = mr;
              
              entrada[k][i] = 0.0;
              printf("Ola6\n");
              for (int j = i+1; j < n; j++)
                  entrada[k][j] -= entrada[i][j] * mr;
            }
        }
    }
    printf("Ola7\n");

    printMatriz(entrada, n, n, stdout);
    printTri(L);
}

/*!
  \brief Realiza a retrossubstituição em L para obter Y
  \param sistema Sistema triangular L
  \param b Coluna da matriz identidade
  \param y Vetor Y a ser obtido
  \param n Tamanho dos vetores
*/
void retrosSubsL(S_tri *sistema, double *b, double *y, int n){
  for (int i = 0; i < n; ++i){
    y[i] = b[i];
    for (int j = 0; j < i; j++)
      y[i] -= sistema->coef[i][j] * y[j];

    if (sistema->coef[i][i] == 0.0){
      fprintf(stderr, "Divisão por 0 encontrada.\n");
      exit(1);
    }
    
    y[i] /= sistema->coef[i][i];
  }
}

/*!
  \brief Realiza a retrossubstituição em U para obter X
  \param a Matriz U
  \param y Vetor Y
  \param x Vetor X a ser obtido
  \param n Tamanho dos vetores
*/
void retrosSubsU(double **a, double *y, double *x, int n){
  for (int i = n-1; i >= 0; i--){
    x[i] = y[i];
    for (int j = i+1; j < n; j++)
      x[i] -= a[i][j] * x[j];
    if (a[i][i] == 0.0){
      fprintf(stderr, "Divisão por 0 encontrada.\n");
      exit(1);
    }
    x[i] /= a[i][i];
  }
}

void imprimeResultados(double **inversa, int n, double tTri, double tY, double tX, double *normas, FILE *saida){
  printMatriz(inversa, n, n, saida);
  fprintf(saida, "Tempo de Triangularização: %lf ms\n", tTri);
  fprintf(saida, "Tempo cálculo de Y: %1.8e ms\n", tY);
  fprintf(saida, "Tempo cálculo de X: %1.8e ms\n", tX);
  fprintf(saida, "Norma L2 do resíduo: ");
  for (int i = 0; i < n; i++){
    fprintf(saida, "%1.8e ", normas[i]);
  }
  fprintf(saida, "\n");
  
}

/*!
  \brief Função para calcular o resíduo entre uma coluna da 
  inversa e a matriz de entrada
  \param matriz Matriz origem
  \param colunaInv Coluna da matriz inversa
  \param res Vetor de resíduos
  \param n Tamanho da matriz e do vetor
  \param index Índice da coluna da matriz identidade
*/
void residuo(double **matriz, double *colunaInv, double *res, unsigned int n, unsigned int index){
  double soma;
  double *colunaIdent = malloc(n*sizeof(double));

  if (colunaIdent == NULL){
    fprintf(stderr, "Erro de alocação\n");
    exit(4);
  }

  for (int i = 0; i < n; i++){
    colunaIdent[i] = 0.0;
  }
  colunaIdent[index] = 1.0;
  
  for (int i = 0; i < n; i++){
    soma = 0.0;
    for (int j = 0; j < n; j++){
      soma += matriz[i][j] * colunaInv[j];
    }
    res[i] = soma - colunaIdent[i];
  }
  free(colunaIdent);
}

/*!
  \brief Calcula as normas L2 para uma matriz e sua inversa
  \param matriz Matriz origem
  \param inversa Matriz inversa
  \param n Tamanho da matriz e do vetor
*/
double *normaL2Residuo(double **matriz, double **inversa, unsigned int n){
  double *res = malloc(n * sizeof(double));
  double *colunaInv = malloc(n * sizeof(double));
  double *normaL2 = malloc(n * sizeof(double));;
  if (res == NULL || colunaInv == NULL || normaL2 == NULL){
    fprintf(stderr, "Erro de alocação\n");
    exit(4);
  }
  for (int i = 0; i < n; i++){
    cpMatrizEmVetor(inversa, colunaInv, n, i);
    residuo(matriz, colunaInv, res, n, i);
    for(int j = 0; j < n; j++)
      normaL2[i] += res[j] * res[j]; 
  }
  // faltou tirar a raiz
  free(res);
  free(colunaInv);
  return normaL2;

}

/*!
  \brief Aplica a fatoração LU para descobrir a matriz inversa
  \param entrada Matriz U
  \param n Tamanho da matriz e do Sistema Triangular
  \param L Sistema Triangular L
*/
int fatoracaoLU(double **entrada, int n, S_tri *L, int pivo, FILE *saida){
  double *Y, *X, *vIdent, **ident, **inversa, **copiaEntrada, *normas;
  double tTriangulacao, tY=0, tX=0;
  Y = malloc(n * sizeof(double));
  X = malloc(n * sizeof(double));
  normas = malloc(n * sizeof(double));
  vIdent = malloc(n * sizeof(double));
  ident = alocaMatriz(n, n);
  copiaEntrada = alocaMatriz(n, n);
  inversa = alocaMatriz(n, n);

  // testa alocacoes
  if (X == NULL || Y == NULL || normas == NULL || vIdent == NULL || 
  ident == NULL || copiaEntrada == NULL || inversa == NULL){
    fprintf(stderr, "Erro de alocação\n");
    exit(4);
  }

  copiaMatriz(entrada, copiaEntrada, n);
  preencheIdent(ident, n);

  fprintf(saida, "%d\n", n);
  printMatriz(entrada, n, n, saida);
  
  tTriangulacao = timestamp();
  //triangulariza(entrada, n, L, pivo, ident);
  tTriangulacao = timestamp() - tTriangulacao;
  
  // pega cada coluna da matriz identidade
  for (int i = 0; i < n; i++){
    cpMatrizEmVetor(ident, vIdent, n, i); // necessário por conta do pivoteamento parcial
    tY += timestamp();
    retrosSubsL(L, vIdent, Y, n);
    tY = timestamp() - tY;
    tX += timestamp();
    retrosSubsU(entrada, Y, X, n);
    tX = timestamp() - tX;
    cpVetorEmMatriz(inversa, X, n, i);
  }
  tY = tY/n;
  tX = tX/n;
  normas = normaL2Residuo(copiaEntrada, inversa, n);
  imprimeResultados(inversa, n, tTriangulacao, tY, tX, normas, saida);

  free(X);
  free(Y);
  free(vIdent);
  free(normas);
  free(inversa);
  free(ident);
  free(copiaEntrada);
}


/*!
  \brief Aplica a fatoração LU para descobrir a matriz inversa
  \param entrada Matriz U
  \param n Tamanho da matriz e do Sistema Triangular
  \param L Sistema Triangular L
*/
int fatoracaoLU2(double **entrada, int n, S_tri *L, int pivo, double *func, FILE *saida){
  double *Y, *X, **copiaEntrada, *normas;
  double tTriangulacao, tY=0, tX=0;
  Y = malloc(n * sizeof(double));
  X = malloc(n * sizeof(double));
  normas = malloc(n * sizeof(double));
  copiaEntrada = alocaMatriz(n, n);
  // testa alocacoes
  if (X == NULL || Y == NULL || normas == NULL || copiaEntrada == NULL){
    fprintf(stderr, "Erro de alocação\n");
    exit(4);
  }

  copiaMatriz(entrada, copiaEntrada, n);

  //fprintf(saida, "%d\n", n);
  //printMatriz(entrada, n, n, saida);
  
  tTriangulacao = timestamp();
  //LIKWID_MARKER_START("Triangulariza");
  triangulariza(entrada, n, L, pivo, func);
  //LIKWID_MARKER_STOP("Triangulariza");

  //LIKWID_MARKER_START("TriangularizaOtimiz");
  //LIKWID_MARKER_STOP("TriangularizaOtimiz");

  tTriangulacao = timestamp() - tTriangulacao;
  retrosSubsL(L, func, Y, n);
  retrosSubsU(entrada, Y, X, n);
  
  //normas = normaL2Residuo(copiaEntrada, inversa, n);
  printVetor(X, n, saida);
  //imprimeResultados(inversa, n, tTriangulacao, tY, tX, normas, saida);

  free(X);
  free(Y);
  free(normas);
  free(copiaEntrada);
}

