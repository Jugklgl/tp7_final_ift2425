//------------------------------------------------------
// module  : Tp2-IFT2425.c
// author  :
// date    :
// version : 1.0
// language: C++
// note    :
//------------------------------------------------------
//

//------------------------------------------------
// FICHIERS INCLUS -------------------------------
//------------------------------------------------
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/*------------------------------------------------*/
/* DEFINITIONS -----------------------------------*/
/*------------------------------------------------*/
#define CARRE(X) ((X)*(X))

//-------------------------
//--- Windows -------------
//-------------------------
#include <X11/Xutil.h>

Display   *display;
int	  screen_num;
int 	  depth;
Window	  root;
Visual*	  visual;
GC	  gc;

/************************************************************************/
/* OPEN_DISPLAY()							*/
/************************************************************************/
int open_display()
{
    if ((display=XOpenDisplay(NULL))==NULL)
    { printf("Connection impossible\n");
        return(-1); }

    else
    { screen_num=DefaultScreen(display);
        visual=DefaultVisual(display,screen_num);
        depth=DefaultDepth(display,screen_num);
        root=RootWindow(display,screen_num);
        return 0; }
}

/************************************************************************/
/* FABRIQUE_WINDOW()							*/
/* Cette fonction cr�e une fenetre X et l'affiche � l'�cran.	        */
/************************************************************************/
Window fabrique_window(char *nom_fen,int x,int y,int width,int height,int zoom)
{
    Window                 win;
    XSizeHints      size_hints;
    XWMHints          wm_hints;
    XClassHint     class_hints;
    XTextProperty  windowName, iconName;

    char *name=nom_fen;

    if(zoom<0) { width/=-zoom; height/=-zoom; }
    if(zoom>0) { width*=zoom;  height*=zoom;  }

    win=XCreateSimpleWindow(display,root,x,y,width,height,1,0,255);

    size_hints.flags=PPosition|PSize|PMinSize;
    size_hints.min_width=width;
    size_hints.min_height=height;

    XStringListToTextProperty(&name,1,&windowName);
    XStringListToTextProperty(&name,1,&iconName);
    wm_hints.initial_state=NormalState;
    wm_hints.input=True;
    wm_hints.flags=StateHint|InputHint;
    class_hints.res_name=nom_fen;
    class_hints.res_class=nom_fen;

    XSetWMProperties(display,win,&windowName,&iconName,
                     NULL,0,&size_hints,&wm_hints,&class_hints);

    gc=XCreateGC(display,win,0,NULL);

    XSelectInput(display,win,ExposureMask|KeyPressMask|ButtonPressMask|
                             ButtonReleaseMask|ButtonMotionMask|PointerMotionHintMask|
                             StructureNotifyMask);

    XMapWindow(display,win);
    return(win);
}

/****************************************************************************/
/* CREE_XIMAGE()							    */
/* Cr�e une XImage � partir d'un tableau de float  (zoom possible)          */
/****************************************************************************/
XImage* cree_Ximage(float** mat,int z,int length,int width)
{
    int lgth,wdth,lig,col,zoom_col,zoom_lig;
    float somme;
    unsigned char	 pix;
    unsigned char* dat;
    XImage* imageX;

    /*Zoom positiv*/
    /*------------*/
    if (z>0)
    {
        lgth=length*z;
        wdth=width*z;

        dat=(unsigned char*)malloc(lgth*(wdth*4)*sizeof(unsigned char));
        if (dat==NULL)
        { printf("Impossible d'allouer de la memoire.");
            exit(-1); }

        for(lig=0;lig<lgth;lig=lig+z) for(col=0;col<wdth;col=col+z)
            {
                pix=(unsigned char)mat[lig/z][col/z];
                for(zoom_lig=0;zoom_lig<z;zoom_lig++) for(zoom_col=0;zoom_col<z;zoom_col++)
                    {
                        dat[((lig+zoom_lig)*wdth*4)+((4*(col+zoom_col))+0)]=pix;
                        dat[((lig+zoom_lig)*wdth*4)+((4*(col+zoom_col))+1)]=pix;
                        dat[((lig+zoom_lig)*wdth*4)+((4*(col+zoom_col))+2)]=pix;
                        dat[((lig+zoom_lig)*wdth*4)+((4*(col+zoom_col))+3)]=pix;
                    }
            }
    } /*--------------------------------------------------------*/

        /*Zoom negatif*/
        /*------------*/
    else
    {
        z=-z;
        lgth=(length/z);
        wdth=(width/z);

        dat=(unsigned char*)malloc(lgth*(wdth*4)*sizeof(unsigned char));
        if (dat==NULL)
        { printf("Impossible d'allouer de la memoire.");
            exit(-1); }

        for(lig=0;lig<(lgth*z);lig=lig+z) for(col=0;col<(wdth*z);col=col+z)
            {
                somme=0.0;
                for(zoom_lig=0;zoom_lig<z;zoom_lig++) for(zoom_col=0;zoom_col<z;zoom_col++)
                        somme+=mat[lig+zoom_lig][col+zoom_col];

                somme/=(z*z);
                dat[((lig/z)*wdth*4)+((4*(col/z))+0)]=(unsigned char)somme;
                dat[((lig/z)*wdth*4)+((4*(col/z))+1)]=(unsigned char)somme;
                dat[((lig/z)*wdth*4)+((4*(col/z))+2)]=(unsigned char)somme;
                dat[((lig/z)*wdth*4)+((4*(col/z))+3)]=(unsigned char)somme;
            }
    } /*--------------------------------------------------------*/

    imageX=XCreateImage(display,visual,depth,ZPixmap,0,(char*)dat,wdth,lgth,16,wdth*4);
    return (imageX);
}

/****************************************************************************/
/* CREE_XIMAGECOUL()							    */
/* Cr�e une XImage � partir d'un tableau 3d de float  (zoom possible)       */
/****************************************************************************/
XImage* cree_XimageCoul(float*** matRVB,int z,int length,int width)
{
    int i;
    int lgth,wdth,lig,col,zoom_col,zoom_lig;
    float somme;
    float sum[3];
    unsigned char	 pixR,pixV,pixB,pixN;
    unsigned char* dat;
    XImage* imageX;

    /*Zoom positif*/
    /*------------*/
    if (z>0)
    {
        lgth=length*z;
        wdth=width*z;

        dat=(unsigned char*)malloc(lgth*(wdth*4)*sizeof(unsigned char));
        if (dat==NULL)
        { printf("Impossible d'allouer de la memoire.");
            exit(-1); }

        for(lig=0;lig<lgth;lig=lig+z) for(col=0;col<wdth;col=col+z)
            {
                pixR=(unsigned char)matRVB[0][lig/z][col/z];
                pixV=(unsigned char)matRVB[1][lig/z][col/z];
                pixB=(unsigned char)matRVB[2][lig/z][col/z];
                somme=(1.0/3.0)*(pixR+pixV+pixB);
                pixN=(unsigned char)somme;

                for(zoom_lig=0;zoom_lig<z;zoom_lig++) for(zoom_col=0;zoom_col<z;zoom_col++)
                    {
                        dat[((lig+zoom_lig)*wdth*4)+((4*(col+zoom_col))+0)]=pixB;
                        dat[((lig+zoom_lig)*wdth*4)+((4*(col+zoom_col))+1)]=pixV;
                        dat[((lig+zoom_lig)*wdth*4)+((4*(col+zoom_col))+2)]=pixR;
                        dat[((lig+zoom_lig)*wdth*4)+((4*(col+zoom_col))+3)]=0;
                    }
            }
    } /*--------------------------------------------------------*/

        /*Zoom negatif*/
        /*------------*/
    else
    {
        z=-z;
        lgth=(length/z);
        wdth=(width/z);

        dat=(unsigned char*)malloc(lgth*(wdth*4)*sizeof(unsigned char));
        if (dat==NULL)
        { printf("Impossible d'allouer de la memoire.");
            exit(-1); }

        for(lig=0;lig<(lgth*z);lig=lig+z) for(col=0;col<(wdth*z);col=col+z)
            {
                sum[0]=sum[1]=sum[2]=0.0;

                for(i=0;i<3;i++)
                    for(zoom_lig=0;zoom_lig<z;zoom_lig++) for(zoom_col=0;zoom_col<z;zoom_col++)
                            sum[i]+=matRVB[i][lig+zoom_lig][col+zoom_col];

                for(i=0;i<3;i++)  sum[i]/=(z*z);

                dat[((lig/z)*wdth*4)+((4*(col/z))+0)]=(unsigned char)sum[1];
                dat[((lig/z)*wdth*4)+((4*(col/z))+1)]=(unsigned char)sum[1];
                dat[((lig/z)*wdth*4)+((4*(col/z))+2)]=(unsigned char)sum[1];
                dat[((lig/z)*wdth*4)+((4*(col/z))+3)]=(unsigned char)sum[1];
            }
    } /*--------------------------------------------------------*/

    imageX=XCreateImage(display,visual,depth,ZPixmap,0,(char*)dat,wdth,lgth,16,wdth*4);
    return (imageX);
}

/****************************************************************************/
/* CREE_XIMAGEWITHMVT()							    */
/* Cr�e une XImage � partir d'une Image N&B et incruste son vecteur de Mvt  */
/****************************************************************************/
XImage* cree_XimageWithMvt(float** matImg,float** vctMvt,int z,int length,int width)
{
    int i;
    int lgth,wdth,lig,col,zoom_col,zoom_lig;
    float sum[3];
    unsigned char	 pixR,pixV,pixB,pixM;
    unsigned char* dat;
    XImage* imageX;

    /*Zoom positif*/
    /*------------*/
    if (z>0)
    {
        lgth=length*z;
        wdth=width*z;

        dat=(unsigned char*)malloc(lgth*(wdth*4)*sizeof(unsigned char));
        if (dat==NULL)
        { printf("Impossible d'allouer de la memoire.");
            exit(-1); }

        for(lig=0;lig<lgth;lig=lig+z) for(col=0;col<wdth;col=col+z)
            {
                pixR=(unsigned char)matImg[lig/z][col/z];
                pixV=(unsigned char)matImg[lig/z][col/z];
                pixB=(unsigned char)matImg[lig/z][col/z];
                pixM=(unsigned char)vctMvt[lig/z][col/z];

                for(zoom_lig=0;zoom_lig<z;zoom_lig++) for(zoom_col=0;zoom_col<z;zoom_col++)
                    { dat[((lig+zoom_lig)*wdth*4)+((4*(col+zoom_col))+0)]=pixB;
                        dat[((lig+zoom_lig)*wdth*4)+((4*(col+zoom_col))+1)]=pixV;
                        dat[((lig+zoom_lig)*wdth*4)+((4*(col+zoom_col))+2)]=pixR;
                        dat[((lig+zoom_lig)*wdth*4)+((4*(col+zoom_col))+3)]=0; }

                for(zoom_lig=0;zoom_lig<z;zoom_lig++) for(zoom_col=0;zoom_col<z;zoom_col++)
                    {
                        if (pixM) { dat[((lig+zoom_lig)*wdth*4)+((4*(col+zoom_col))+0)]=0;
                            dat[((lig+zoom_lig)*wdth*4)+((4*(col+zoom_col))+1)]=255;
                            dat[((lig+zoom_lig)*wdth*4)+((4*(col+zoom_col))+2)]=0; }
                    }
            }
    } /*--------------------------------------------------------*/

    imageX=XCreateImage(display,visual,depth,ZPixmap,0,(char*)dat,wdth,lgth,16,wdth*4);
    return (imageX);
}

//-------------------------//
//--- Fonctions Pour TP ---//
//-------------------------//
//---------------------------------------------------------
//  Alloue de la memoire pour une matrice 1d de float
//----------------------------------------------------------
float* fmatrix_allocate_1d(int hsize)
{
    float* matrix;
    matrix=(float*)malloc(sizeof(float)*hsize); return matrix; }

//----------------------------------------------------------
//  Alloue de la memoire pour une matrice 2d de float
//----------------------------------------------------------
float** fmatrix_allocate_2d(int vsize,int hsize)
{
    int i;
    float** matrix;
    float* imptr;

    matrix=(float**)malloc(sizeof(float*)*vsize);
    imptr=(float*)malloc(sizeof(float)*(hsize)*(vsize));
    for(i=0;i<vsize;i++,imptr+=hsize) matrix[i]=imptr;
    return matrix;
}

//----------------------------------------------------------
// Alloue de la memoire pour une matrice 3d de float
//----------------------------------------------------------
float*** fmatrix_allocate_3d(int dsize,int vsize,int hsize)
{
    int i;
    float*** matrix;

    matrix=(float***)malloc(sizeof(float**)*dsize);

    for(i=0;i<dsize;i++)
        matrix[i]=fmatrix_allocate_2d(vsize,hsize);
    return matrix;
}

//----------------------------------------------------------
// Libere la memoire de la matrice 1d de float
//----------------------------------------------------------
void free_fmatrix_1d(float* pmat)
{
    free(pmat);
}

//----------------------------------------------------------
// Libere la memoire de la matrice 2d de float
//----------------------------------------------------------
void free_fmatrix_2d(float** pmat)
{
    free(pmat[0]);
    free(pmat);
}

//----------------------------------------------------------
// libere la memoire de la matrice 3d de float
//----------------------------------------------------------
void free_fmatrix_3d(float*** pmat,int dsize)
{
    int i;
    for(i=0;i<dsize;i++)
    { free (pmat[i][0]);
        free (pmat[i]); }
    free(pmat);
}

//----------------------------------------------------------
// Chargement de l'image de nom <name> (en pgm)
//----------------------------------------------------------
float** LoadImagePgm(char* name,int *length,int *width)
{
    int i,j,k;
    unsigned char var;
    float** mat;
    int ta1,ta2,ta3;
    FILE *fic;
    char buff[100],stringTmp1[100],stringTmp2[100];

    //>Ouverture_Fichier
    strcpy(buff,name);
    printf("\n  > Ouverture de %s",buff);

    fic=fopen(buff,"r");
    if (fic==NULL)
    { printf("\n- Grave erreur a l'ouverture de %s  -\n",buff);
        exit(-1); }

    //>Recuperation_Entete
    fgets(stringTmp1,100,fic);
    for(;;) { fread(&var,1,1,fic); if (var==35) fgets(stringTmp2,100,fic);
        else break; }
    fseek(fic,-1,SEEK_CUR);
    fscanf(fic,"%d %d",&ta1,&ta2);
    fscanf(fic,"%d\n",&ta3);

    //>Enregistrement_AllocationMemoire
    *length=ta2;
    *width=ta1;
    mat=fmatrix_allocate_2d(*length,*width);

    //>Chargement Image
    for(i=0;i<*length;i++) for(j=0;j<*width;j++)
        { fread(&var,1,1,fic);
            mat[i][j]=var; }

    //>End
    fclose(fic);
    return(mat);
}

//----------------------------------------------------------
// Sauvegarde de l'image de nom <name> au format pgm
//----------------------------------------------------------
void SaveImagePgm(char* name,float** mat,int lgth,int wdth)
{
    int i,j;
    char buff[300];
    FILE* fic;

    //>Extension
    strcpy(buff,name);
    strcat(buff,".pgm");

    //>Ouverture fichier
    fic=fopen(buff,"wb");
    if (fic==NULL)
    { printf("Probleme dans la sauvegarde de %s",buff);
        exit(-1); }
    printf("\n Sauvegarde [%s] (format pgm)",buff);

    //>Sauvegarde de l'entete
    fprintf(fic,"P5");
    fprintf(fic,"\n# IMG Module");
    fprintf(fic,"\n%d %d",wdth,lgth);
    fprintf(fic,"\n255\n");

    //>Enregistrement--
    for(i=0;i<lgth;i++) for(j=0;j<wdth;j++) fprintf(fic,"%c",(char)mat[i][j]);

    //>Fermeture fichier
    fclose(fic);
}

//----------------------------------------------------------
// Sauvegarde de l'image de nom <name> au format ppm
//----------------------------------------------------------
void SaveImagePpm(char* name,float** img,float** vct,int lgth,int wdth)
{
    int i,j;
    char buff[300];
    FILE* fic;

    //>Extension
    strcpy(buff,name);
    strcat(buff,".ppm");

    //>Ouverture fichier
    fic=fopen(buff,"wb");
    if (fic==NULL)
    { printf("Probleme dans la sauvegarde de %s",buff);
        exit(-1); }
    printf("\n Sauvegarde [%s] (format ppm)",buff);

    //>Sauvegarde de l'entete
    fprintf(fic,"P6");
    fprintf(fic,"\n# IMG Module");
    fprintf(fic,"\n%d %d",wdth,lgth);
    fprintf(fic,"\n255\n");

    //>Enregistrement--
    for(i=0;i<lgth;i++) for(j=0;j<wdth;j++)
        {
            if (!vct[i][j])
            { fprintf(fic,"%c",(char)img[i][j]);
                fprintf(fic,"%c",(char)img[i][j]);
                fprintf(fic,"%c",(char)img[i][j]); }
            else
            { fprintf(fic,"%c",(char)0);
                fprintf(fic,"%c",(char)vct[i][j]);
                fprintf(fic,"%c",(char)0); }
        }

    //>Fermeture fichier
    fclose(fic);
}

//----------------------------------------------------------
// DrawLine[Img] (rowbeg,colbeg)-->(rowend,colend)
//----------------------------------------------------------
void DrawLine(float** Img,int colbeg,int rowbeg,int colend,int rowend,int wdth,int lgth)
{
    int k;
    float cord,pente;
    float incrow,inccol;
    float diffrow=(float)(rowend-rowbeg);
    float diffcol=(float)(colend-colbeg);

    //Boucles
    if (fabs(diffrow)>=fabs(diffcol))
    { pente=(float)(diffcol)/(float)(diffrow);
        incrow=diffrow/fabs(diffrow);
        cord=colbeg;

        if (rowend>=rowbeg) for(k=rowbeg;k<=rowend;k++)
            { if ((k>0)&&(cord>0)&&(k<lgth)&&(cord<wdth)) Img[k][(int)cord]=255;
                cord+=pente; }

        if (rowbeg>rowend) for(k=rowbeg;k>=rowend;k--)
            { if ((k>0)&&(cord>0)&&(k<lgth)&&(cord<wdth)) Img[k][(int)cord]=255;
                cord-=pente; }

        cord=colend;
        k=rowend;
        if ((k>0)&&(cord>0)&&(k<lgth)&&(cord<wdth)) Img[k][(int)cord]=255;
    }

    if (fabs(diffcol)>fabs(diffrow))
    { pente=(float)(diffrow)/(float)(diffcol);
        inccol=diffcol/fabs(diffcol);
        cord=rowbeg;

        if (colend>=colbeg) for(k=colbeg;k<=colend;k++)
            { if ((k>0)&&(cord>0)&&(k<wdth)&&(cord<lgth)) Img[(int)cord][k]=255;
                cord+=pente;}

        if (colbeg>colend) for(k=colbeg;k>=colend;k--)
            { if ((k>0)&&(cord>0)&&(k<wdth)&&(cord<lgth))  Img[(int)cord][k]=255;
                cord-=pente; }

        cord=rowend;
        k=colend;
        if ((k>0)&&(cord>0)&&(k<lgth)&&(cord<wdth)) Img[(int)cord][k]=255;
    }
}

//----------------------------------------------------------
// PutArrowOnImg
//----------------------------------------------------------
void PutArrowOnImg(float** Img,int lgth,int wdth,int PosX,int PosY,float IncPosX,float IncPosY)
{
    double Theta;
    double PX,PY;
    double NxtPosX,NxtPosY;

    double PI=3.141592653;
    double FACT=5.0;
    float LGTH_MIN=1.0;
    float LGTH_MIN_ARROW=4.0;

    double IncPosX_= IncPosX*FACT;
    double IncPosY_= IncPosY*FACT;
    double LgthArrow=sqrt((IncPosX_*IncPosX_)+(IncPosY_*IncPosY_));
    double Sz=LgthArrow/2.5;

    if (IncPosX==0) Theta=PI/2;
    else Theta=atan2(IncPosY,IncPosX);

    NxtPosX=PosX+(IncPosX_);
    NxtPosY=PosY+(IncPosY_);

    if (LgthArrow>LGTH_MIN)
    {
        DrawLine(Img,PosX,PosY,NxtPosX,NxtPosY,wdth,lgth);

        if (LgthArrow>LGTH_MIN_ARROW)
        {
            Theta=atan2(PosY-NxtPosY,PosX-NxtPosX);
            PX=(int)(NxtPosX + Sz * cos(Theta + PI/4.0));
            PY=(int)(NxtPosY + Sz * sin(Theta + PI/4.0));
            DrawLine(Img,PX,PY,NxtPosX,NxtPosY,wdth,lgth);
            PX=(int)(NxtPosX + Sz * cos(Theta - PI/4.0));
            PY=(int)(NxtPosY + Sz * sin(Theta - PI/4.0));
            DrawLine(Img,PX,PY,NxtPosX,NxtPosY,wdth,lgth);
        }
    }
}

//----------------------------------------------------------
// ConvertVelocityFieldInArrowField
//----------------------------------------------------------
void ConvertVelocityFieldInArrowField(float** ImgOptFlot,float** Vx,float** Vy,int lgth,int wdth,int INC)
{
    int i,j;

    printf("\n Convert_Velocity_Field ...");
    fflush(stdout);
    for(i=0;i<lgth;i++) for(j=0;j<wdth;j++) ImgOptFlot[i][j]=0.0;

    for(i=INC;i<(lgth-INC);i+=INC) for(j=INC;j<(wdth-INC);j+=INC)
            PutArrowOnImg(ImgOptFlot,lgth,wdth,j,i,Vx[i][j],Vy[i][j]);
}


//-------------------------//
//--- Vos Fonctions Ici ---//
//-------------------------//


float Gauss(float a, float b){
    return (float) expf(-sqrtf(a*a+b*b) / 4.0);
}


//----------------------------------------------------------
//----------------------------------------------------------
// PROGRAMME PRINCIPAL -------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
int main(int argc,char** argv)
{
    int   i,j,k,l;
    int   length,width;
    float alpha;

    float** img1;
    float** img2;
    float** Ix;
    float** Iy;
    float** It;
    float** ImgOptFlot;
    float** OptFl_Vx;
    float** OptFl_Vy;





    //>Pour Xwindow
    XEvent ev;
    Window win_ppicture1,win_ppicture2,win_ppicture3;
    XImage *x_ppicture1,*x_ppicture2,*x_ppicture3;
    char   nomfen_ppicture1[100],nomfen_ppicture2[100],nomfen_ppicture3[100];

    //>Lis Arguments
    if(argc<3)
    { printf("\n\t Usage: %s image1 image2",argv[0]);
        printf("\n\t Exemple: %s rubikseq0.pgm rubikseq1.pgm",argv[0]);
        printf("\n\t Exemple: %s salesman0.pgm salesman1.pgm",argv[0]);
        printf("\n\t Exemple: %s carfront0.pgm carfront1.pgm",argv[0]);
        printf("\n\n\n");
        return 0; }
    int zoom=2;

    //>Load Images_&_Parametre
    img1=LoadImagePgm(argv[1],&length,&width);
    img2=LoadImagePgm(argv[2],&length,&width);

    //>Allocation Memoire
    ImgOptFlot=fmatrix_allocate_2d(length,width);
    OptFl_Vx=fmatrix_allocate_2d(length,width);
    OptFl_Vy=fmatrix_allocate_2d(length,width);
    Ix=fmatrix_allocate_2d(length,width);
    Iy=fmatrix_allocate_2d(length,width);
    It=fmatrix_allocate_2d(length,width);


    //Init
    for(i=0;i<length;i++) for(j=0;j<width;j++)
        { ImgOptFlot[i][j]=0.0;
            OptFl_Vx[i][j]=0.0;
            OptFl_Vy[i][j]=0.0; }

    //> Rempli OptFlot (par un flot optique aleatoire)
    //> les cinqs lignes suivantes sont a mettre en commentaire
    { float posx=1.3*cos(3.0*3.14*100);
        float posy=1.3*sin(3.0*3.14*100);
        for(i=5;i<length-5;i++) for(j=5;j<width-5;j++)
            {  OptFl_Vx[i][j]=posx;
                OptFl_Vy[i][j]=posy; } }

    //------------------------------------------------------------
    // Estimation du Flux Optique de Lucas & Kanade [par Moindres Carr�s]
    //
    // L'image 1 et 2 sont enregistr�e dans les tableau 2D Img1[][] et Img2[][]
    // Ix, Iy, It sont des tableaux 2D de m�me dimension que l'image qui ont �t� allou�es
    // OptFl_Vx, OptFl_Vy qui contiendra le flot optique
    //
    //-----------------------------------------------------------


    // programmer ici .....

    const int n = 1225;

    float x=0,y=0,a=0,b=0,c=0,d=0,e=0,f=0;
    float** wi;
    wi = fmatrix_allocate_2d(length,width);

    float** matG;

    matG = fmatrix_allocate_2d(2,2);


    // init
    for (i = 0; i<2; i++) for(j=0;j<2;j++){
            matG[i][j] = 0.0;
        }



    // calcul de Ix, It et It

    for(i=0;i<length;i++) for(j=0;j<width;j++)
        {
            if(i+1 == length && j+1 == width)
            {
                Ix[i][j] = 0.0;
                Iy[i][j] = 0.0;
                It[i][j] = img2[i][j]-img1[i][j];
            }
            else if(i+1 == length){

                Ix[i][j] = (img1[i][j+1]-img1[i][j] + img2[i][j+1]-img2[i][j]) / 2.0;
                Iy[i][j] = 0.0;
                It[i][j] = (img2[i][j]-img1[i][j] + img2[i][j+1]-img1[i][j+1]) / 2.0;
            }
            else if(j+1 == width){

                Ix[i][j] = 0.0;
                Iy[i][j] = (img1[i+1][j]-img1[i][j] + img2[i+1][j]-img2[i][j]) / 2.0;
                It[i][j] = (img2[i][j]-img1[i][j] + img2[i+1][j]-img1[i+1][j]) / 2.0;
            }
            else{

                Ix[i][j] = (img1[i][j+1]-img1[i][j] + img1[i+1][j+1]-img1[i+1][j]
                            +img2[i][j+1]-img2[i][j] + img2[i+1][j+1]-img2[i+1][j]) / 4.0;
                Iy[i][j] = (img1[i+1][j]-img1[i][j] + img1[i+1][j+1]-img1[i][j+1]
                            +img2[i+1][j]-img2[i][j] + img2[i+1][j+1]-img2[i][j+1]) / 4.0;
                It[i][j] = (img2[i][j]-img1[i][j] + img2[i+1][j]-img1[i+1][j]
                            +img2[i][j+1]-img1[i][j+1] + img2[i+1][j+1]-img1[i+1][j+1]) / 4.0;
            }
        }

/*
    // calcul des wi
    for(i=0;i<length;i++) for(j=0;j<width;j++){

        wi[i][j] = Gauss()



    }



    for(i=0; i<n;i++) for(j=0; j<n;j++){

        a += Ix[i][j]*Ix[i][j];
        b += Ix[i][j]*Iy[i][j];
        c = b ;
        d += Iy[i][j]*Iy[i][j] ;
        e += Ix[i][j]*It[i][j];
        f += Iy[i][j]*It[i][j];

        }

    e = -e; f = -f;

    float divisuer = a*d - b*c;

    if(divisuer != 0){
        x = (e*d-b*f) / divisuer;
        y = (a*f-e*c) / divisuer;
    }

    a=0.0; b=a; c=a; d=a; e=a; f=a;
    float tmp_e = 0.0, tmp_f = 0.0;

    for(i=0; i<n;i++) for(j=0; j<n;j++){

            wi += Gauss(x-Ix[i][j], y-Iy[i][j]);
            a += w*Ix[i][j]*Ix[i][j];
            b += w*Ix[i][j]*Iy[i][j];
            c = b ;
            d += w*Iy[i][j]*Iy[i][j] ;
            e += w*Ix[i][j]*It[i][j];
            f += w*Iy[i][j]*It[i][j];

            // vecteur de droite
            tmp_e = -e;
            tmp_f = -f;

            // calcul de la matrice inverse de (A^T W A)
            matG[0][0] =  d / divisuer;
            matG[0][1] = -b / divisuer;
            matG[1][0] = -c / divisuer;
            matG[1][1] =  a / divisuer;


            OptFl_Vx[i][j] = (matG[0][0]*tmp_e) + (matG[0][1]*tmp_f);
            OptFl_Vy[i][j] = (matG[1][0]*tmp_e) + (matG[1][0]*tmp_f);


            tmp_e = 0.0; tmp_f = 0.0;




        }




*/





    //Convert {Vx[i][j],Vy[i][j]} -> {Array Of Vector}
    ConvertVelocityFieldInArrowField(ImgOptFlot,OptFl_Vx,OptFl_Vy,length,width,7);

    //Sauvegarde
    printf("\n");
    SaveImagePpm("Optical_Flot_WithImg",img2,ImgOptFlot,length,width);
    SaveImagePgm("Optical_Flot",ImgOptFlot,length,width);


//--------------------------------------------------------------------------------
//---------------- Visu sous XWINDOW ---------------------------------------------
//--------------------------------------------------------------------------------
    //Info
    printf("\n\n Evolution du Flot Optique estime: ");

    //>Window creation
    if (open_display()<0) printf("Graphical Session Error");
    sprintf(nomfen_ppicture1,"Image Sequence");
    sprintf(nomfen_ppicture2,"Optical Flow");
    sprintf(nomfen_ppicture3,"Image + Optical Flow");
    win_ppicture1=fabrique_window(nomfen_ppicture1,10,10,width,length,zoom);
    win_ppicture2=fabrique_window(nomfen_ppicture2,12+(zoom*width),10,width,length,zoom);
    win_ppicture3=fabrique_window(nomfen_ppicture3,12+(zoom*width),10,width,length,zoom);

    //>Visu Sequences
    printf("\n");
    for(k=0;k<200;k++)
    {
        printf("\r [%d/200]",k);
        fflush(stdout);
        if (k%2)       x_ppicture1=cree_Ximage(img1,zoom,length,width);
        else           x_ppicture1=cree_Ximage(img2,zoom,length,width);
        x_ppicture2=cree_Ximage(ImgOptFlot,zoom,length,width);
        x_ppicture3=cree_XimageWithMvt(img2,ImgOptFlot,zoom,length,width);

        XPutImage(display,win_ppicture1,gc,x_ppicture1,0,0,0,0,
                  x_ppicture1->width,x_ppicture1->height);
        XPutImage(display,win_ppicture2,gc,x_ppicture2,0,0,0,0,
                  x_ppicture2->width,x_ppicture2->height);
        XPutImage(display,win_ppicture3,gc,x_ppicture3,0,0,0,0,
                  x_ppicture3->width,x_ppicture3->height);

        usleep(100000); //si votre machine est lente mettre un nombre moins grand
        if (k==199);
        { XDestroyImage(x_ppicture1);
            XDestroyImage(x_ppicture2);
            XDestroyImage(x_ppicture3); }
    }


//--------------- End Graphical Session --------------------
//----------------------------------------------------------

    //Liberation Memoire
    if (OptFl_Vx)  free_fmatrix_2d(OptFl_Vx);
    if (OptFl_Vy)  free_fmatrix_2d(OptFl_Vy);
    if (Ix)  free_fmatrix_2d(Ix);
    if (Iy)  free_fmatrix_2d(Iy);
    if (It)  free_fmatrix_2d(It);
    if (matG) free_fmatrix_2d(matG);
    if (wi) free_fmatrix_2d(wi);

    //Return
    printf("\n C'est fini... \n");
    return 0;
}


