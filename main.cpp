/* 
 * File:   main.cpp
 * Author: victor
 *
 * Created on March 15, 2016, 5:13 PM
 */

#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "config.h"

using namespace std;

/*
 * 
 */

int readRoot(){
    DIR* rootDir;
    DIR* audioDir;
    struct dirent* ent;
    struct dirent* entChild;
    FILE* listFile;
    char audioDirPath[100];
    char line[200];
    char path[120];
    FILE* refFile;
    FILE* targetFile;
    FILE* UBMlist;
    FILE* testFile;
    FILE* testList;
    int docCount, objCount=0,trainCount=0,testCount=0,base=0,spkCount=0;
    char* aux;
    char name[20];
    
    rootDir=opendir(ROOT_DIR);
    if(rootDir==NULL){
        perror("Não conseguiu abrir o diretorio raiz");
        return -1;
    }
    listFile=fopen(LIST_PATH, "w");
    if(listFile==NULL){
        perror("Não conseguiu criar arquivo de lista");
        return -2;
    }
    sprintf(path, "%s/lst/UBM.lst", HOME_DIR);
    UBMlist = fopen(path, "w");
    if(UBMlist == NULL){
        perror("Não conseguiu criar arquivo UBM de lista");
        return -3;
    }
    sprintf(path, "%s/trecRef.txt", HOME_DIR);
    refFile=fopen(path,"w");
    if(refFile==NULL){
        perror("Não conseguiu criar arquivo refFile");
        return -5;
    }
    sprintf(path, "%s/ndx/trainModel.ndx", HOME_DIR);
    targetFile = fopen(path, "w");
    if(targetFile == NULL){
        perror("Não conseguiu criar arquivo UBM de lista");
        return -4;
    }
    sprintf(path, "%s/lst/test.lst", HOME_DIR);
    testList = fopen(path, "w");
    if(testList == NULL){
        perror("Não conseguiu criar arquivo test de lista");
        return -5;
    }
    while((ent=readdir(rootDir))!=NULL){
        if(strlen(ent->d_name)>2){
            spkCount++;
            sprintf(audioDirPath, "%s/%s", ROOT_DIR, ent->d_name);
            audioDir=opendir(audioDirPath);
            if(audioDir==NULL){
                perror("Não conseguiu abrir o diretorio filho");
                continue;
            }
//            fprintf(targetFile, "obj%i ", spkCount);
            base+=testCount;
            docCount=0;
            testCount=0;
            while((entChild=readdir(audioDir))!=NULL){
                if(entChild->d_name[strlen(entChild->d_name)-3]=='w' &&
                entChild->d_name[strlen(entChild->d_name)-2]=='a' &&
                entChild->d_name[strlen(entChild->d_name)-1]=='v'){
                    docCount++;
                    aux=strrchr(entChild->d_name,'.');
                    strncpy(name,entChild->d_name,strlen(entChild->d_name)-4);
                    name[strlen(entChild->d_name)-4]=0;
                    if(docCount<=7){
                        fprintf(UBMlist, "%s\n", name);
//                        fprintf(targetFile, "%s ", name);
                        trainCount++;
                    }else{
                        fprintf(targetFile, "obj%i %s\n", objCount,name);
                        fprintf(testList,"%s\n",name);
//                        fprintf(refFile, "%i 0 %i 1\n",spkCount,totalDocs);
                        testCount++;
                        objCount++;
                    }
                    sprintf(line, "%s/%s\n", audioDirPath, entChild->d_name);
                    fputs(line, listFile);
                }
            }
            for(int i=base;i<base+testCount;i++){
                fprintf(refFile, "%i 0 %i 1\n",spkCount,i);   
            }
            closedir(audioDir);
        }
    }
    closedir(rootDir);
    fclose(listFile);
    fclose(UBMlist);
    fclose(targetFile);
    fclose(testList);
    fclose(refFile);
    
    testList = fopen(path, "r");
    if(testList == NULL){
        perror("Não conseguiu abrir arquivo test de lista");
        return -6;
    }
    sprintf(path, "%s/ndx/computeTestGMMTarget.ndx", HOME_DIR);
    testFile = fopen(path, "w");
    if(testFile == NULL){
        perror("Não conseguiu criar arquivo UBM de lista");
        return -5;
    }
    while(fgets(line,sizeof(line), testList)!=NULL){
        line[strlen(line)-1]=0;
        fprintf(testFile,"%s ", line);
        for(int i=1;i<objCount;i++){
            fprintf(testFile, "spk%i ",i);
        }
        fprintf(testFile, "spk%i\n",objCount);
    }
    fclose(testList);
    fclose(testFile);
}

int makeHCopyScript(){
    char scriptPath[100];
    char line[300], newLine[400];
    char mfcFile[60];
    char* aux;
    FILE* list;
    FILE* script;
    
    
    list = fopen(LIST_PATH,"r");
    if(list==NULL){
        perror("Não conseguiu abrir o arquivo de lista");
        return -3;
    }
    
    sprintf(scriptPath, "%s/data/%s.scp", HOME_DIR, CORPUS_NAME);
    script = fopen(scriptPath, "w");
    if(script==NULL){
        perror("Não conseguiu criar arquivo de script");
        return -4;
    }
    
    while(fgets(line, sizeof(line), list)!=NULL){
        line[strlen(line)-1]=0;
        aux=strrchr(line, '/');
        strcpy(mfcFile, aux+1);
        strcpy(mfcFile+(strlen(mfcFile)-3), "mfc");
        sprintf(newLine, "%s %s/data/feat/%s\n", line, HOME_DIR,mfcFile);
        //printf("line: %s\n", newLine);
        fputs(newLine, script);
    }
    fclose(script);
    fclose(list);
    
    
    return 0;
}

int makeSysFile(){
    FILE* sysFile;
    FILE* distFile;
    char fileName[50];
    char line[50];
    int queryCount=1,docCount=0;
    
    
    sprintf(fileName, "%s/distanceMatrix", HOME_DIR);
    distFile = fopen(fileName, "r");
    if(distFile == NULL){
        perror("Não conseguiu abrir o arquivo de distancias");
        return -1;
    }
    sprintf(fileName, "%s/trecSys.txt", HOME_DIR);
    sysFile = fopen(fileName, "w");
    if(sysFile==NULL){
        perror("Não conseguiu criar arquivo de sistema");
        return -2;
    }
    
    while(fgets(line, sizeof(line), distFile)!=NULL){
        fprintf(sysFile, "%i 0 %i 1 %f 0\n",queryCount,docCount,atof(line));
        queryCount++;
        if(queryCount>35){
            queryCount=1;
            docCount++;
        }
    }
    fclose(sysFile);
    fclose(distFile);
    return 0;
}

int makeDistanceMatrix(){
    FILE* resFile;
    FILE* distFile;
    char fileName[50];
    char line[50];
    float dist;
    
    sprintf(fileName, "%s/res/target_gmm.res", HOME_DIR);
    resFile=fopen(fileName, "r");
    if(resFile==NULL){
        perror("Não conseguiu abrir arquivo de resposta");
        return -1;
    }
    sprintf(fileName, "%s/distanceMatrix", HOME_DIR);
    distFile=fopen(fileName, "w");
    if(resFile==NULL){
        perror("Não conseguiu criar arquivo de distancia");
        return -1;
    }
    while(fgets(line, sizeof(line), resFile)!=NULL){
        strtok(line, " ");
        strtok(NULL, " ");
        strtok(NULL, " ");
        strtok(NULL, " ");
        dist = atof(strtok(NULL, " "));
        fprintf(distFile, "%f\n", dist);
    }
    fclose(resFile);
    fclose(distFile);
}

int main(int argc, char** argv) {
    //readRoot();
    //makeHCopyScript();
    makeDistanceMatrix();
    makeSysFile();
    return 0;
}

