#########################################################
# CGB, 20100718
#########################################################

python.method.call <- function( py.object, py.method, ..., instance.name = "" ){
    i = instance.name
    python.call( paste( py.object, py.method, sep = "." ), ..., instance.name=i )
}

