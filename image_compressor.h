typedef struct pixfreq
	{
	     int pix,larrloc,rarrloc;
	     float freq;
	     struct pixfreq *left, *right;
             char* code;
	}pixfreq;
	
typedef struct huffcode
	{
	   int pix,arrloc;
	   float freq;
	}huffcode;



void image_array(int** image,int height,int width,FILE* image_file);
void init_histogram(int hist[],int** image,int height,int width);
int find_non_zero_occurence(int hist[]);
int max_len_code(int hist[],int height,int width);
void init_pixfreq_huffcode(pixfreq** pix_freq,huffcode** huffcodes,int nnz);
void fill_data_to_pixfreq_huffcode(pixfreq* pix_freq,huffcode* huffcodes,int max, int height,int width,int hist[],int totalnnz);
void sort_huffcode(huffcode* huffcodes,int nnz);
void buildTree(huffcode* huffcodes, pixfreq* pix_freq,int nnz);
void build_huffman_code(pixfreq* pix_freq,int nnz,int totalnnz);
void compressImgFile(int fd1, int fd2);
