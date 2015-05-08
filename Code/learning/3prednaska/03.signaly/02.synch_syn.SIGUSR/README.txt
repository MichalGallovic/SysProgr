Programy demonstruju synchronizaciu procesov rodic-detsky proces s pomocou synchronneho odchytenie synchronizacnych
signalov SIGUSER1, SIGUSER2. Synchronizacia procesov je vo vypisoch - za vypisom procesu, 
ktory signal vysle by mal nasledovat vypis procesu, ktory signal obdrzi. 
Procesy cakaju na signal na funkcii sigwait. Funkcia sigwait pracuje korektne len ak su signaly blokovane.
To znamena, ze ak proces na signal necaka tento sa zoradi do fronty nedorucenych signalov odkial sa neskor
v pripade ked proces zacne vykonavat funkciu sigwait vyberie a nestrati sa.