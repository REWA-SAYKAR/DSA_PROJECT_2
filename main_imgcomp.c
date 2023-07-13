#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<fcntl.h>//For LINUX file handling
#include<unistd.h>//for LINUX file handling(w)
#include "image_compressor.h"
int main(int argc, char *argv[]){	
	char *filename;
	filename = argv[1];
	int i,j,r,c;
	//char filename[] = "project.bmp";
	int data=0,offset,hbytes,width,height;
	long bmpsize=0,bmpdataoff=0,bpp=0;
	int **image;
	int temp=0;

	//Reading the BMP File
	FILE *image_file;
	image_file = fopen(filename,"rb");
	if(image_file==NULL)
	{
		printf("Error Opening File!!");
		exit(1);
	}
		//Set file position of the stream to the beginning
		offset = 0;
		fseek(image_file,offset,SEEK_SET);
		for(i=0;i<2;i++)
		{
			fread(&data,1,1,image_file);
			printf("%c",data);
		}
		fread(&bmpsize,4,1,image_file);
		offset = 10;
		fseek(image_file,offset,SEEK_SET);
		fread(&bmpdataoff,4,1,image_file);
		fread(&hbytes,4,1,image_file);
		fread(&width,4,1,image_file);
		fread(&height,4,1,image_file);
		fseek(image_file,2,SEEK_CUR);
		fread(&bpp,2,1,image_file);
		fseek(image_file,bmpdataoff,SEEK_SET);
		
		//Creating Image array
		image = (int **)malloc(height*sizeof(int *));
		image_array(image,height,width,image_file);
		fclose(image_file);
		
		//Finding the probability of occurence
	        int hist[256];
	        init_histogram(hist,image,height,width);
	        
	        //Finding number of non-zero occurences
	        int nnz=find_non_zero_occurence(hist);
	        //printf("%d",nnz);

                int max=max_len_code(hist,height,width);
                
                //initialize the array of pixfeq and huffcode structure
                pixfreq *pix_freq;
                huffcode *huffcodes;
                int totalnnz=2*nnz-1;
                init_pixfreq_huffcode(&pix_freq,&huffcodes,nnz);
                fill_data_to_pixfreq_huffcode(pix_freq,huffcodes,max,height,width,hist,totalnnz);
                
                //Sorting w.r.to probability of occurence
                sort_huffcode(huffcodes,nnz);
                
                //Building Huffman Tree
                buildTree(huffcodes,pix_freq, nnz);
	        
	        //Building huffman code
	        build_huffman_code(pix_freq,nnz,totalnnz);
	       
	      int pix_val ;  
	      FILE *imagehuff = fopen("encoded_image.txt","wb");
	      for(int r=0;r<height;r++)
		for(int c=0;c<width;c++)
		{
		        pix_val = image[r][c];
			for(int i=0;i<nnz;i++)
				if(pix_val == pix_freq[i].pix)
					fprintf(imagehuff,"%s",pix_freq[i].code);
		}
		fclose(imagehuff);

		//now compressing the file by storing it bit by bit using bit manipulation technique
		
		char to_be_comp[] = "encoded_image.txt";
		int fd1 = open(to_be_comp, O_RDONLY);
		if (fd1 == -1) {
    			perror("Open Failed For Input File:\n");
    			exit(1);
		}

		// Creating output file in write mode
		char sample_compressed[] = "sample_img_compressed.txt";
		int fd2 = open(sample_compressed,O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
		if (fd2 == -1) {
    			perror("Open Failed For comp Output File:\n");
    			exit(1);
		}
		compressImgFile(fd1,fd2);
		close(fd1);
}

