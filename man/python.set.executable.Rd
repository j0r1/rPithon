\name{python.set.executable}
\alias{python.set.executable}
\title{ python.set.executable }
\description{
  Sets the path to the Python executable to be used.
}
\usage{
  python.set.executable( path.to.python.executable )
}
\arguments{
  \item{ path.to.python.executable }{ Path to the Python executable that should be used. }
}
\details{
  Sets the path to the Python executable to be used. This function must be used before
  any other function that needs Python, otherwise it will have no further effect.

  If the function is not called, the program "python" will be executed and communicated
  with. Note that this is only possible if the PATH environment variable contains the
  Python path. On most systems this default should be okay, but the function may be
  explicitly set which Python executable should be used.
}
\value{
    None.
}

\examples{

python.set.executable( "/usr/bin/python" )

}

\keyword{ manip }

