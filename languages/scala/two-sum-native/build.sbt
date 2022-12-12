scalaVersion := "3.1.3"

enablePlugins(ScalaNativePlugin)

//
// Enable unit tests.
// See https://scala-native.org/en/stable/user/testing.html
//
libraryDependencies += "org.scala-native" %%% "junit-runtime" % "0.4.9"
addCompilerPlugin("org.scala-native" % "junit-plugin" % "0.4.9" cross CrossVersion.full)
testOptions += Tests.Argument(TestFrameworks.JUnit, "-a", "-s", "-v")
