# Code from rPython 0.0-5, by Carlos J. Gil Bellosta
# Small modifications by Jori Liesenborgs

pithon.assign <- function(var.name, value, ..., instance.name = ""){

    #value <- toJSON( value )
    value <- toJSON( value, collapse = "", digits = 32, ...)

    # Creating the call

    pithon.command <- c( 
        paste( var.name , "='", value, "'",  sep = " " ),
        paste( var.name , "= json.loads(", var.name, ")", sep = "" ),
        paste( "if len(",  var.name , ") == 1:", sep = "" ),
        paste( "    ", var.name, "=", var.name, "[0]" ) 
    )

    pithon.command <- paste( pithon.command, collapse = "\n" )

    pithon.exec( pithon.command, TRUE, instance.name)
    invisible( NULL )
}

