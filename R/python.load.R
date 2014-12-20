#########################################################
# CGB, 20120801
#########################################################

python.load <- function( file, get.exception = TRUE, instance.name = "" ){
    code <- readLines( file )
    python.exec( code, get.exception, instance.name )
}



