#include <stdio.h>
#include "image/image.h"
#include "image/image_filters.h"
#include "utils/utils.h"
#include <unistd.h>
#include "image_pipeline.h"
/**
 * Imprime el uso del programa.
 */
void print_usage(){
    printf("Uso: image_pipeline -c <num_im> -u <umbral> -n <umbral> -m <path> [-b]\n");
    printf("Argumentos:\n");
    printf("    -c <num_im> : Numero de imagenes a procesar.\n");
    printf("    -u <umbral> : Umbral de binarizacion. Numero entre 0 y 255.\n");
    printf("    -n <umbral> : Porcentaje para umbral de clasificacion. Numero entre 0 y 100\n");
    printf("    -m <path>   : Ruta al archivo de la mascara. El archivo de la mascara debe ser de 3x3 enteros.\n");
    printf("    -b          : Mostrar resultados de clasificacion.\n");
}

/**
 * Comprueba los parametros de entrada y establece sus valores a traves de pasos por referencia.
 * @param argc : Cantidad de argumentos recibidos.
 * @param argv : Lista de argumentos recibidos.
 * @param number_of_images : Puntero donde se guarda el numero de imagenes.
 * @param binarization_threshold : Puntero donde se guarda el umbral de binarizacion.
 * @param nearly_black_threshold : Puntero donde se guarda el umbral de clasificacion.
 * @param mask_path : Puntero donde se guarda la ruta al archivo con la mascara.
 * @param verbose : Puntero donde se guarda el estado de la bandera "b" (1 = SI, 0 = NO).
 * @return 1 en caso de exito, 0 en caso contrario.
 */
int get_arguments(int argc, char **argv, int* number_of_images, int* binarization_threshold, double* nearly_black_threshold,
                  char** mask_path, int* verbose){
    // Comprobaciones iniciales de acuerdo al numero de argumentos recibidos
    if (argc < 9){
        printf("Faltan argumentos\n");
        print_usage();
        return 0;
    } else if (argc > 10){
        printf("Demasiados argumentos\n");
        print_usage();
        return 0;
    }

    int option;
    char found_args[5] = {'0', '0', '0', '0', '0'};
    int found_args_i = 0;
    while ((option = getopt(argc, argv, ":c:u:n:m:b")) != -1){
        //printf("option: %c\n", option);
        if (option == '?'){
            printf("Opcion invalida: %s\n", argv[optind-1]);
            print_usage();
            return 0;
        } else if (option == ':'){
            printf("Falta argumento: %s <arg>\n", argv[optind-1]);
            print_usage();
            return 0;
        } else if (optarg != NULL && optarg[0] == '-'){
            printf("Falta argumento: %s <arg>\n", argv[optind-2]);
            print_usage();
            return 0;
        }
        switch (option) {
            // Para cada uno de los argumentos leidos, comprobar que no sea repetido.
            case 'c':
                if (contains(found_args, (char)option, 5) == 0){
                    found_args[found_args_i] = (char)option;
                    found_args_i++;
                    //printf("c, optarg:%s\n", optarg);
                    *number_of_images = atoi(optarg);
                }
                break;
            case 'u':
                if (contains(found_args, (char)option, 5) == 0){
                    found_args[found_args_i] = (char)option;
                    found_args_i++;
                    //printf("u, optarg:%s\n", optarg);
                    *binarization_threshold = atoi(optarg);
                }
                break;
            case 'n':
                if (contains(found_args, (char)option, 5) == 0){
                    found_args[found_args_i] = (char)option;
                    found_args_i++;
                    //printf("n, optarg:%s\n", optarg);
                    char* ptr;
                    *nearly_black_threshold = strtod(optarg, &ptr);
                }
                break;
            case 'm':
                if (contains(found_args, (char)option, 5) == 0){
                    found_args[found_args_i] = (char)option;
                    found_args_i++;
                    //printf("m, optarg:%s\n", optarg);
                    *mask_path = (char*)calloc(sizeof(char), strlen(optarg)+1);
                    strcpy(*mask_path, optarg);
                    mask_path[strlen(optarg)-1] = '\0';
                }
                break;
            case 'b':
                if (contains(found_args, (char)option, 5) == 0){
                    found_args[found_args_i] = (char)option;
                    found_args_i++;
                    *verbose = 1;
                }
                break;
            default:
                print_usage();
        }
    }
    if (found_args_i == 4 && contains(found_args, 'b', 5)){
        if (*mask_path != NULL) free(*mask_path);
        print_usage();
        return 0;
    }
    if (found_args_i < 4){
        if (*mask_path != NULL) free(*mask_path);
        print_usage();
        return 0;
    }
    return 1;
}

/**
 * Procesa una imagen
 * @param binarization_threshold : valor entre 0 y 255 para binarizar la imagen.
 * @param nearly_black_threshold : Valor entre 0 y 100 para clasificar la imagen (porcentaje de pixeles blancos).
 * @param img_path : Ruta a la imagen en disco.
 * @param mask : Mascara para aplicar el filtro de Laplace.
 * @param dest_img_path : Ruta donde guardar la imagen binarizada resultante.
 * @return 1 en caso de que resulte nearly_black, 0 en no nearly_black. -1 en caso de error.
 */
int pipeline(int binarization_threshold, double nearly_black_threshold, char* img_path, Mask* mask, char* dest_img_path){
    // Lectura
    Image* img = (Image*)malloc(sizeof(Image));
    load_image(img, img_path);

    // Filtro gray scale
    Image* gray_scale_img = image_to_gray_scale(img);
    free_image(img);
    free(img);

    // Filtro Laplace
    Image* laplace_img = laplace_filter(gray_scale_img, mask);
    free_image(gray_scale_img);
    free(gray_scale_img);

    // Binarizacion
    Image* binarized_img = binarize_image(laplace_img, binarization_threshold);
    if (binarized_img == NULL) {
        printf("Error al binarizar imagen '%s'. Posiblemente el umbral sea incorrecto\n", img_path);
        exit(-1);
    }
    free_image(laplace_img);
    free(laplace_img);

    // Escritura
    save_image(binarized_img, dest_img_path);

    // Clasificacion
    int nb = nearly_black(binarized_img, nearly_black_threshold);
    free_image(binarized_img);
    free(binarized_img);
    return nb;
}

void print_results(Result* results, int res_size){
    // Encontrar el nombre de la imagen mas largo
    int longest_size = 0;
    for (int i=0; i<res_size; i++){
        int l = (int)strlen(results[i].name);
        if (l > longest_size) longest_size = l;
    }
    int image_column_size = longest_size+6;
    int nearly_black_column_size = 16;
    int left_margin = 0, right_margin = 0;
    printf("+"); print_n_chars('-', image_column_size); printf("+"); print_n_chars('-', nearly_black_column_size); printf("+\n");
    string_center("Image", image_column_size, &left_margin, &right_margin);
    printf("|"); print_n_chars(' ', left_margin); printf("%s", "Image"); print_n_chars(' ', right_margin);
    string_center("Nearly black", nearly_black_column_size, &left_margin, &right_margin);
    printf("|"); print_n_chars(' ', left_margin); printf("Nearly black"); print_n_chars(' ', right_margin); printf("|\n");
    printf("+"); print_n_chars('-', image_column_size); printf("+"); print_n_chars('-', nearly_black_column_size); printf("+\n");

    for (int i=0; i<res_size; i++){
        printf("|");
        char* name = results[i].name;
        char ans[5];
        if (results[i].classification == NEARLY_BLACK) strcpy(ans, "YES\0");
        else strcpy(ans, "NO\0");
        string_center(name, image_column_size, &left_margin, &right_margin);
        print_n_chars(' ', left_margin); printf("%s", name); print_n_chars(' ', right_margin); printf("|");
        string_center(ans, nearly_black_column_size, &left_margin, &right_margin);
        print_n_chars(' ', left_margin); printf("%s", ans); print_n_chars(' ', right_margin); printf("|\n");
        printf("+"); print_n_chars('-', image_column_size); printf("+"); print_n_chars('-', nearly_black_column_size); printf("+\n");
    }
}

void print_pipe_results(char* img_path, int np){
    int image_column_size = (int)strlen(img_path)+6;
    int nearly_black_column_size = 16;
    int left_margin = 0, right_margin = 0;
    char ans[20];
    fprintf(stderr,"|");
    if (np == 1) strcpy(ans, "NEARLY BLACK\0");
    else strcpy(ans, "NOT NEARLY BLACK\0");
    string_center(img_path, image_column_size, &left_margin, &right_margin);
    print_n_chars_stderr(' ', left_margin); fprintf(stderr,"%s", img_path); print_n_chars_stderr(' ', right_margin); fprintf(stderr,"|");
    string_center(ans, nearly_black_column_size, &left_margin, &right_margin);
    print_n_chars_stderr(' ', left_margin); fprintf(stderr,"%s", ans); print_n_chars_stderr(' ', right_margin); fprintf(stderr,"|\n");
}
