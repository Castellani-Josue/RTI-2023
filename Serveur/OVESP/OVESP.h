#ifndef OVESP_H
#define OVESP_H

#define NB_MAX_CLIENTS 5


bool OVESP(char* requete,char* reponse, int socket);
//bool OVESP_Login(const char* user,const char* motDePasse);
//void OVESP_Logout();
void OVESP_Close();


#endif