#########################################################
# CGB, 20100718
#########################################################

pithon.method.call <- function( py.object, py.method, ..., instance.name = "" ){
    instname = instance.name
    pithon.call( paste( py.object, py.method, sep = "." ), ..., instance.name=instname )
}

