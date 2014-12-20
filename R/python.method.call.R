#########################################################
# CGB, 20100718
#########################################################

python.method.call <- function( py.object, py.method, ..., instance.name = "" ){
    instname = instance.name
    python.call( paste( py.object, py.method, sep = "." ), ..., instance.name=instname )
}

