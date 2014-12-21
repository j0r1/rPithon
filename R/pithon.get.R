#########################################################
# CGB, 20100716, created
#########################################################

pithon.get <- function(var.name, instance.name = "" ){

    ret <- .C( "py_get_var", var.name, instance.name, found = integer(1), result = character(1), PACKAGE = "rPithon" )

    if(!ret$found)
        stop(paste("Couldn't retrieve variable: ", ret$result, sep=""))
        
    ret <- fromJSON(ret$result)

    if( length( ret ) == 1 ) ret <- ret[[1]]
    ret
}

