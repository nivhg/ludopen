#-------------------------------------------------
#
# Project created by QtCreator 2012-05-04T12:10:40
#
#-------------------------------------------------
#DEFINES   = QT_NO_DEBUG_OUTPUT\
#            QT_NO_WARNING_OUTPUT
QT        = widgets sql network core printsupport script multimediawidgets designer xml
TARGET    = LudOpen
TEMPLATE  = app
RESOURCES = ImagesIcones.qrc
# Si je veux que l'application démarre dans une fenêtre de shell
# pour avoir les messages d'erreur
#CONFIG   += CONSOLE

INCLUDEPATH = $$PWD/libs/ssh/

SOURCES += \
    main.cpp \
    f_statistiques.cpp \
    f_retour.cpp \
    f_reservation.cpp \
    f_plusdestatistiques.cpp \
    f_paiement.cpp \
    f_membres.cpp \
    f_mainwindow.cpp \
    f_listejeux.cpp \
    f_jeux.cpp \
    f_historiquejeux.cpp \
    f_emprunt.cpp \
    f_detailsjeux.cpp \
    f_declarerintervention.cpp \
    f_ajoutsuppmodifjeux.cpp \
    f_ajoutsuppmodiffournisseurediteurs.cpp \
    f_ajoutercoticarte.cpp \
    f_abonnements.cpp \
    f_preferences.cpp \
    f_retards.cpp \
    Courriel.cpp \
    f_listemembres.cpp \
    f_postit.cpp \
    f_popupclesttem.cpp \
    f_imprimerfichecompletejeu.cpp \
    f_imprimeretiquettejeu.cpp \
    f_listereservations.cpp \
    d_image.cpp \
    lb_image.cpp \
    d_ajoutsuppmodiffournisseursediteurs.cpp \
    acces_fichier_http.cpp \
    securefileuploader.cpp \
    fonctions_globale.cpp \
    majeur.cpp \
    f_malles.cpp \
    suggest.cpp \
    searchbox.cpp \
    tableviewtooltipmodifier.cpp \
    f_imprimermalle.cpp \
    d_releve.cpp \
    d_popupcode.cpp \
    f_panier.cpp \
    f_imprimerfacture.cpp \
    d_ajoutelementpanier.cpp \
    d_aide.cpp \
    d_resamisdecote.cpp \
    bcrypt.c \
    crypt_blowfish/crypt_blowfish.c \
    crypt_blowfish/crypt_gensalt.c \
    crypt_blowfish/wrapper.c \
    d_lienjeux.cpp \
    spinboxdelegate.cpp \
    w_contenujeu.cpp \
    w_historiquemaintenance.cpp \
    http_xml_api.cpp

HEADERS  += \
    f_statistiques.h \
    f_retour.h \
    f_reservation.h \
    f_plusdestatistiques.h \
    f_paiement.h \
    f_membres.h \
    f_mainwindow.h \
    f_listejeux.h \
    f_jeux.h \
    f_historiquejeux.h \
    f_emprunt.h \
    f_detailsjeux.h \
    f_declarerintervention.h \
    f_ajoutsuppmodifjeux.h \
    f_ajoutsuppmodiffournisseursediteurs.h \
    f_ajoutercoticarte.h \
    f_abonnements.h \
    TitreType.h \
    f_preferences.h \
    f_retards.h \
    Courriel.h \
    f_listemembres.h \
    f_postit.h \
    f_popupclesttem.h \
    f_imprimerfichecompletejeu.h \
    f_imprimeretiquettejeu.h \
    f_listereservations.h \
    d_image.h \
    lb_image.h \
    d_ajoutsuppmodiffournisseursediteurs.h \
    acces_fichier_http.h \
    securefileuploader.h \
    fonctions_globale.h \
    majeur.h \
    f_malles.h \
    suggest.h \
    searchbox.h \
    tableviewtooltipmodifier.h \
    f_imprimermalle.h \
    d_releve.h \
    d_popupcode.h \
    f_panier.h \
    f_imprimerfacture.h \
    d_ajoutelementpanier.h \
    d_aide.h \
    d_resamisdecote.h \
    bcrypt.h \
    crypt_blowfish/crypt.h \
    crypt_blowfish/crypt_blowfish.h \
    crypt_blowfish/crypt_gensalt.h \
    crypt_blowfish/ow-crypt.h \
    d_lienjeux.h \
    spinboxdelegate.h \
    w_contenujeu.h \
    w_historiquemaintenance.h \
    http_xml_api.h

FORMS    += \
    f_statistiques.ui \
    f_retour.ui \
    f_reservation.ui \
    f_plusdestatistiques.ui \
    f_paiement.ui \
    f_membres.ui \
    f_mainwindow.ui \
    f_listejeux.ui \
    f_jeux.ui \
    f_historiquejeux.ui \
    f_emprunt.ui \
    f_detailsjeux.ui \
    f_declarerintervention.ui \
    f_ajoutsuppmodifjeux.ui \
    f_ajoutsuppmodiffournisseursediteurs.ui \
    f_ajoutercoticarte.ui \
    f_abonnements.ui \
    f_preferences.ui \
    f_retards.ui \
    f_listemembres.ui \
    f_postit.ui \
    f_popupclesttem.ui \
    f_imprimerfichecompletejeu.ui \
    f_imprimeretiquettejeu.ui \
    f_listereservations.ui \
    d_image.ui \
    d_ajoutsuppmodiffournisseursediteurs.ui \
    f_malles.ui \
    f_imprimermalle.ui \
    d_releve.ui \
    d_popupcode.ui \
    f_panier.ui \
    f_imprimerfacture.ui \
    d_ajoutelementpanier.ui \
    d_aide.ui \
    d_resamisdecote.ui \
    d_lienjeux.ui \
    w_contenujeu.ui \
    w_historiquemaintenance.ui

VERSION = "2.05"
VERSTR = '\\"$${VERSION}\\"'  # place quotes around the version string
DEFINES += VER=\"$${VERSTR}\" # create a VER macro containing the version string

include(qssh.pri) ## Required for IDE_LIBRARY_PATH and qtLibraryName
LIBS += -L$$IDE_LIBRARY_PATH -l$$qtLibraryName(botan-2) -l$$qtLibraryName(QSsh)
