/* 
 * File:   config.h
 * Author: victor
 *
 * Created on March 15, 2016, 5:19 PM
 */

#ifndef CONFIG_H
#define	CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

    #define ROOT_DIR "/home/victor/Documents/Mestrado/Projeto/datasets/youtubeDataset"
    #define CORPUS_NAME "YouTube"
    #define REF_DIR "/home/victor/Documents/Mestrado/Projeto/GMM/refDir"
    #define HOME_DIR REF_DIR "/" CORPUS_NAME
    #define LIST_PATH HOME_DIR "/" CORPUS_NAME ".txt"
    #define REF_FILE HOME_DIR "/trecRefGenVq.txt"
    #define CONFIG_DIR HOME_DIR "/cfg" 



#ifdef	__cplusplus
}
#endif

#endif	/* CONFIG_H */

