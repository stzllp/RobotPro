#ifndef MP3_PLAYER_H
#define MP3_PLAYER_H

void MP3_PlayerInit(void);
void SendToDAC(int samprate, int nchannel, void*data, int size);
void PlayMP3(char filename[]);

#endif