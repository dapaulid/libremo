#!/bin/bash -e

if [ ! -d "$BUILDDIR" ]; then
	ci/prebuild.sh
fi

ci/build.sh
ci/test.sh
ci/coverage.sh
ci/example.sh
