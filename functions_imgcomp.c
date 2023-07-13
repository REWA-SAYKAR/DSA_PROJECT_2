#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<fcntl.h>//for LINUX file handling
#include<unistd.h>//for LINUX file handling(w)
#include "image_compressor.h"

int codelen(char *code)
{
	int l=0;
	while(*(code+l)!='\0')
		l++;
	return l;
}

void strconcat(char *str, char *parentcode, char add)
{
	int i=0;
	while(*(parentcode+i)!='\0')
	{
		*(str+i)=*(parentcode+i);
		i++;
	}
	if(add!='2')
	{
		str[i]=add;
		str[i+1]='\0';
	}
	else
		str[i]='\0';
}

int fib(int n)
{
    if (n <= 1)
        return n;
    return fib(n - 1) + fib(n - 2);
}
void image_array(int**image,int height,int width, FILE* image_file){
         int temp=0;
         for(int i=0;i<height;i++){
	          image[i] = (int *)malloc(width*sizeof(int));
	  }
          //Reading the BMP File into Image Array
	 for(int r=0;r<height;r++){
	      for(int c=0;c<width;c++)
		 {
		   fread(&temp,3,1,image_file);
		    temp = temp&0x0000FF;
		    image[r][c] = temp;
		}
	    }
	}
void init_histogram(int hist[],int** image,int height,int width){
	   for(int i=0;i<256;i++)
	   {
		hist[i]=0;
	   }
	   for(int i=0;i<height;i++)
		for(int j=0;j<width;j++)
			hist[image[i][j]]+=1;
	}
	
	int find_non_zero_occurence(int hist[]){ 
	   int nnz=0;
	   for(int i=0;i<256;i++)
	    {
		if(hist[i]!=0)
			nnz+=1;
	    }
	    return nnz;
	}
	 // Calculating max length of code word
	int max_len_code(int hist[],int height,int width){
              	float p = 1.0,ptemp;
	        for(int i=0;i<256;i++){
		     ptemp = (hist[i]/(float)(height*width));
		     if (ptemp>0 && ptemp<=p)
			p = ptemp;
	         }
                 int i = 0;
                 while ((1 / p) > fib(i))
                           i++;
	         int maxcodelen = i-3;
	         return maxcodelen;
	}
	
	//Defining Structures pixfreq
void init_pixfreq_huffcode(pixfreq** pix_freq,huffcode** huffcodes,int nnz){
	      int totalnnz = 2*nnz-1;
	      *pix_freq = (pixfreq *)malloc(sizeof(pixfreq)*totalnnz);
	      *huffcodes = (huffcode *)malloc(sizeof(huffcode)*nnz);
	}
	
void fill_data_to_pixfreq_huffcode(pixfreq* pix_freq,huffcode* huffcodes,int max, int height,int width,int hist[],int totalnnz){
	     int totpix = height*width;
	     for(int i=0;i<totalnnz;i++){
	         pix_freq[i].code=(char*)malloc(sizeof(char)*max);
	         }
	      int j=0;
	      float tempfreq;
	      for(int i=0;i<256;i++){
		  if(hist[i]!=0){
			huffcodes[j].pix = i;
			pix_freq[j].pix = i;
			huffcodes[j].arrloc = j;
			tempfreq = (float)hist[i]/(float)totpix;
			pix_freq[j].freq = tempfreq;
			huffcodes[j].freq = tempfreq;
			pix_freq[j].left = NULL;
			pix_freq[j].right = NULL;
			pix_freq[j].code[0] = '\0';
			j++;
		}
	}
   }
	//Sorting the histogram
	//Sorting w.r.to probability of occurence
void sort_huffcode(huffcode* huffcodes,int nnz){
	     huffcode temphuff;
	     for(int i=0;i<nnz;i++){
		 for(int j=i+1;j<nnz;j++)
	  	{
			if(huffcodes[i].freq < huffcodes[j].freq)
	    		{
				temphuff = huffcodes[i];
				huffcodes[i] = huffcodes[j];
				huffcodes[j] = temphuff;
	    		}
	  	}
	}
	}
	
	//Building Huffman Tree
void buildTree(huffcode* huffcodes, pixfreq* pix_freq,int nnz){
	     float sumprob;
	     int sumpix;
	     int n=0,k=0,l=0;
	     printf("nnz:: %d\n",nnz);
	     int nextnode = nnz;
	     while(n<nnz-1)
	      { 
	  	//Adding the lowest two probabilities
		sumprob = huffcodes[nnz-n-1].freq+huffcodes[nnz-n-2].freq;
		sumpix = huffcodes[nnz-n-1].pix+huffcodes[nnz-n-2].pix;
		int i=0;
		//New position of the combined node
		while(sumprob<=huffcodes[i].freq)
			i++;
		//Appending to the Heap Array
		pix_freq[nextnode].pix = sumpix;
		pix_freq[nextnode].freq = sumprob;
		pix_freq[nextnode].left = &pix_freq[huffcodes[nnz-n-2].arrloc];
		pix_freq[nextnode].right = &pix_freq[huffcodes[nnz-n-1].arrloc];
		pix_freq[nextnode].code[0] = '\0';
		//Sorting the new array		
		for(int k=nnz;k>=0;k--)
		{
			if(k==i)
			{
				huffcodes[k].pix=sumpix;
				huffcodes[k].freq=sumprob;
				huffcodes[k].arrloc = nextnode;
			}
			else if(k>i)
			{
				huffcodes[k] = huffcodes[k-1];
			}
		}
		n+=1;
		nextnode+=1;
	}
    }
	
	//Assigning Code through backtracking
void build_huffman_code(pixfreq* pix_freq,int nnz,int totalnnz){
	     char left = '0';
	     char right = '1';
	     int index;
	      for(int i=totalnnz-1;i>=nnz;i--)
	       {
		    if(pix_freq[i].left!=NULL)
		     {
			strconcat(pix_freq[i].left->code,pix_freq[i].code,left);
			}
		    if(pix_freq[i].right!=NULL) 
		     {
			strconcat(pix_freq[i].right->code,pix_freq[i].code,right);
		       }
	       }
	//naming the last node as ROOT
	       strconcat(pix_freq[totalnnz-1].code,"root",'2');
}

//noe the function to compress the encoded file
void compressImgFile(int fd1, int fd2){
	unsigned char a;
	char n;
	int h = 0, i;
	while(read(fd1, &n, sizeof(char)) != 0) {
                if (h < 7) {
                    if (n == '1') {
                        a++;
                        a = a << 1;
                        h++;
                    }
                    else if (n == '0') {
                        a = a << 1;
                        h++;
                    }
                }
                else if (h == 7) {
                    if (n == 1){
                        a++;
                        h = 0;
                    }
                    else {
                        h = 0;
                    }
                    /*for(int i=0;i<8;i++){
			    printf("%d",!!((a<<i) & 0x80));
		    }
		    printf("\n");*/

		    write(fd2, &a, sizeof(char));
                    a = 0;
                }
	}
    
    	for (i = 0; i < 7 - h; i++) {
        	a = a << 1;
    	}
    	write(fd2, &a, sizeof(char));
    	printf("\nFile compressed successfully\n");
}
	
