#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

int main(void) {
    int w,h,c; // largura, altura, número de canais de cores
    unsigned char *img = stbi_load("imagem.jpg", &w, &h, &c, 0);
    
    if(img == NULL) {
        printf("Nao foi possivel ler a imagem\n");
        exit(1);
    }
		
    char * final = malloc(w*h*c); //alocando espaço para a nova imagem
    int filtro[3][3] = { {-1,-1,-1},{-1,8,-1},{-1,-1,-1}};
    
    for (int j=0;j<h;j++) {
		for (int i=0;i<w;i++) { // varre toda a imagem
			for (int canal=0;canal<c;canal++) { //Varre os canais de cores
				int novo = 0;
				for (int ii=-1;ii<2;ii++) { 
					for (int jj=-1;jj<2;jj++) { // se movimente na matriz de convolução
						int cordx = ii+i;
						int cordy = jj+j;
						int tmp = 0;
						if (cordx>=0 && cordx<w && cordy>=0 && cordy<h) {
							tmp = *(img+canal+(cordx*c+cordy*w*c)); //pega a informação do pixel
						}
						novo += tmp*filtro[ii+1][jj+1]; //aplica o filtro
					}
				}
				if (novo<0) novo =0;
				if (novo>255) novo = 255;
				*(final+canal+(i*c+j*w*c))=novo; // Cria nova imagem
			}
		}
	}

    stbi_write_jpg("imagem2.jpg",w,h,c,final,100); //salva imagem
	stbi_image_free(img);
	free(final);
}