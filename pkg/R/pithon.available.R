pithon.available <- function(instance.name = "")
{
	avail <- FALSE

	tryCatch({
		pithon.get("sys.version")
		avail <- TRUE
	}, error = function(e) { })

	return(avail)
}
