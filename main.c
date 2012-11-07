/* 
 * File:   main.c
 * Author: mevin
 *
 * Created on November 4, 2012, 10:28 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "bass.h"
#include <string.h>
#include <glob.h>

HSTREAM stream;
char key;
static int status = 0;
static int playcount = 1;
static int toatalcount = 0;
static char path[515] = "";

void help() {
    printf("p\tPlay/Pause\n");
    printf("s\tStop and exit player\n");
    printf("e\tSame as s exit player\n");
    printf("i\tNow playing song info\n");
    printf("n\tNext Song\n");
    printf("b\tPrevious Song\n");
    printf("s\tHelp\n");
    printf("a\tAbout\n");
}

int init_file_list() {
    glob_t data;
    int FLAG = 0;
    strcat(path, "*.mp3");
    printf("%s\n", path);
    if (glob(path, 0, NULL, &data))
        FLAG = 1;
    int i;
    FILE *file = fopen("./playlist.m2o", "wb");
    if (file != NULL) {
        for (i = 0; i < data.gl_pathc; i++) {
            strcat(data.gl_pathv[i], "\n");
            fputs(data.gl_pathv[i], file);
            toatalcount++;
        }
        FLAG = 1;
    }

    globfree(&data);
    fclose(file);
    if (FLAG == 1)
        return 1;
    else
        return 0;
}

char* getFile() {
    FILE *pl = fopen("./playlist.m2o", "rb");
    char *buffer, array[257];
    int count = 0;
    if (pl != NULL) {
        while (count < playcount) {
            buffer = fgets(array, sizeof (array), pl);
            count++;
        }
    }
    fclose(pl);
    return buffer;
}

void exit_player(char *status, char *error) {
    printf("%s : %s \n", status, error);
    BASS_StreamFree(stream);
    BASS_Free();
    exit(1);
}

void getInfo(int info) {
    TAG_ID3 *tag = (TAG_ID3*) BASS_ChannelGetTags(stream, BASS_TAG_ID3);
    printf("Title : \t%s\n", tag->title);
    if (info == 0) {
        printf("Artist : \t%s\n", tag->artist);
        printf("Album : \t%s\n", tag->album);
        printf("Year : \t%s\n", tag->year);
    }
    return;
}

void stop() {
    BASS_ChannelStop(stream);
    BASS_StreamFree(stream);
    status = 2;
}

void play(char *file) {
    if (status != 0)
        stop();
    stream = BASS_StreamCreateFile(FALSE, file, 0, 0, 0);
    BASS_ChannelPlay(stream, 0);
    status = 1;
    printf("Now Playing :\n%s\n", file);
    //  getInfo(1); //bug: causes to crash when songs with huge titles comein :|
}

int init() {
    if (BASS_Init(-1, 48000, 0, 0, 0)) {
        return 1;
    } else {
        return 0;
    }
}

void play_list() {
    char array[257];
    strcpy(array, getFile());
    int len = strlen(array) - 1;
    if (array[len] == '\n')
        array[len] = 0;
    play(array);
}

void play_cur_dir() {
    if (init_file_list()) {
        if (status == 1)
            stop();

        play_list();
    } else
        exit_player("ERROR", "Playlist cannot be made");
}

void shell() {
    printf("Type h for help\n");
    printf("mp3 > ");

    while (key = getchar()) {
        switch (key) {
            case 'e':
                exit_player("Closed", "Program Exiting");
                break;
            case 'i':
                getInfo(0);
                break;
            case 's':
                stop();
                break;
            case 'p':
                if (status == 1) {
                    BASS_ChannelPause(stream);
                    status = 0;
                } else if (status == 0) {
                    BASS_ChannelPlay(stream, 0);
                    status = 1;
                } else if (status == 2) {
                    play(getFile());
                }
                break;
            case 'n':
                if (playcount < toatalcount)
                    playcount++;
                else
                    playcount = 1;
                play_list();
                break;  
            case 'b':
                if (playcount <= 1)
                    playcount = toatalcount;
                else
                    playcount--;

                play_list();
                break;
            case 'h':
                help();
                break;
            case 'a':
                printf("M2O2 player v0.2\n");
                break;
            default:
                printf("mp3 > ");
        }
    }
}

int main(int argc, char *argv[]) {
    switch (argc) {
        case 1:
            printf("Loading Playlist %d\n", argc);
            init();
            play_cur_dir();
            break;
        case 2:
            if (init())
                play(argv[1]);
            else
                exit_player("ERROR", "Unable to Load BASS Library");
            break;
        case 3:
            init();
            strcpy(path, argv[1]);
            printf("%s", path);
            play_cur_dir();
            break;
        default:
            exit_player("ERROR", "Too many parameters");
            printf("--------syntax-----------");
            printf("play file_name.mp3\t-- to play a mp3\n");
            printf("play folder_name folder\t-- to play a folder\n");

            break;
    }

    shell();

}