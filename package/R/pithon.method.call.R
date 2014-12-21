# Code from rPython 0.0-5, by Carlos J. Gil Bellosta
# Small modifications by Jori Liesenborgs

pithon.method.call <- function( py.object, py.method, ..., instance.name = "" ){
    instname = instance.name
    pithon.call( paste( py.object, py.method, sep = "." ), ..., instance.name=instname )
}

