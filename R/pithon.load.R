#########################################################
# CGB, 20120801
#########################################################

pithon.load <- function( file, get.exception = TRUE, instance.name = "" ){
    code <- readLines( file )
    pithon.exec( code, get.exception, instance.name )
}



