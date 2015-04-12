\name{pithon.set.default.executable}
\alias{pithon.set.default.executable}
\title{ pithon.set.default.executable }
\description{
  Sets the path to the Python executable to be used by default, for all newly started instances.
}
\usage{
  pithon.set.default.executable( path.to.python.executable )
}
\arguments{
  \item{ path.to.python.executable }{ path to the Python executable that should be used. }
}
\details{
  Sets the path to the Python executable to be used by default for all newly started
  instances.  If the function is not called, the result from 'find_python_cmd' (from
  the 'findpython' package) will be used as default.
}
\value{
    None.
}

\examples{

pithon.set.default.executable( "c:/Python34/python.exe" )

}

\keyword{ manip }

