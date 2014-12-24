pithon.available <- function(instance.name = "")
{
	avail <- FALSE

	tryCatch({
		iname <- instance.name
		pithon.get("sys.version", instance.name=iname)
		avail <- TRUE
	}, error = function(e) { })

	return(avail)
}
