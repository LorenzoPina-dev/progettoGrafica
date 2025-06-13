Instruzioni per run:

    Aprire MSYS

    Navigare nella cartella in cui si è scaricato il progetto

    cd 15c-Caricamento-Modelli

    make
    
    ./bin/caricamento_modelli.exe


Progetto per corso di informatica grafica
Implementazioni:
    Edit mode

    Selezione mesh

    Trasformazioni su oggetti

    Trasformazioni su parti di mesh

    Creazione di un gestore per tastiera

    Creazione di un gestore per mouse

    Creazione di una super classe sceneObject


Gesture implementate:

    tasto destro del mouse per camera:

        drag: rotazione camera

        shift+drag: sposta la camera

        ctrl+drag: zoom in+out

    tasto centrale del mouse con trasformazione selezionata:

        drag: azione su oggetto

    g: per traslazione

    r: rotazione

    s: scaling

    x: lock asse x

    y: lock asse y

    z: lock asse z

    shift+x: lock yz

    shift+y: lock xz

    shift+z: lock xy

    (ripremere stessa trasformazione o asse cmabia sistema di riferimento)

    spazio: reset

    esc: chiude il programma

    tab: edit mode

    1: selezione vertici

    2: selezione edge

    3: selezione facce

    1: diminuisce luce ambientale

    2: aumenta luce ambientale

    3: diminuisce luce diffusiva

    4: aumenta luce diffusiva

    5: diminuisce luce speculare

    6: aumenta luce speculare

    7: diminuisce shine speculare

    8: aumenta shine speculare


    +: aumenta il fov

    -: diminuisce il fov

    $: aumenta il far plane

    %: diminuisce il far plane

    !: aumenta il near plane

    ": diminuisce il near plane


    shift+s: aggiunge un teschio alla scena

    shift+t: aggiunge una teliera alla scena

    shift+F: aggiunge un fiore alla scena

    shift+d: aggiunge un drago alla scena

    shift+b: aggiunge una scarpa alla scena

    canc: elimina oggetto selezionato


    freccia in alto: aumenta rotazione automatica verticale

    freccia in basso: riduce rotazione automatica verticale

    freccia a sinistra: aumenta rotazione automatica orizzontale

    freccia a destra: diminuisce rotazione automatica orizzontale
    
  
  
