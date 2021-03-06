#ifndef TITRETYPE_H
#define TITRETYPE_H

/** @struct Type
 *  @brief Indique l'id du type et le nom correspondant
 */
typedef struct {
    unsigned int id ; ///< Id du type
    QString sType ;   ///< Nom du type correspondant à l'id
}Type ;

/** @struct Titre
 *  @brief Indique l'id du titre et le nom correspondant
 */
typedef struct {
    unsigned int id ;///< Id du titre
    QString sTitre ; ///< Nom du titre correspondant à l'id
    int nJeuxAutorises ; ///< Nombre de jeux autorisés
}Titre ;

/** @struct StatutDuJeu
 *  @brief Indique l'id du StatutDuJeu et le nom correspondant
 */
typedef struct {
    unsigned int id ;       ///< Id du StatutDuJeu
    QString sStatutDuJeu ;  ///< Nom du StatutDuJeucorrespondant à l'id
}StatutDuJeu ;


#endif // TITRETYPE_H
