# Code from rPython 0.0-5, by Carlos J. Gil Bellosta
# Small modifications by Jori Liesenborgs

pithon.load <- function( file, get.exception = TRUE, instance.name = "" ){
    code <- readLines( file )
    pithon.exec( code, get.exception, instance.name )
}



