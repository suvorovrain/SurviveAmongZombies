#!/bin/sh -e

LOG_DIR="./valgrind_logs"
mkdir -p "$LOG_DIR"

check_program() {
	prog="$1"
	prog_name=$(basename "$prog")
	logfile="${LOG_DIR}/valgrind_${prog_name}.log"

	echo "Checking $prog for memory leaks..."

	valgrind \
		--tool=memcheck \
		--leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		--errors-for-leak-kinds=all \
		--error-exitcode=1 \
		--log-file="$logfile" \
		./"$prog"

	echo "Checking $prog for thread safety..."

	valgrind \
		--tool=helgrind \
		--error-exitcode=1 \
		--log-file="$logfile" \
		./"$prog"

	if ! grep -q "ERROR SUMMARY: 0 errors" "$logfile"; then
		echo "Memory leaks detected in $prog!"
		cat "$logfile"
		return 1
	fi

	if grep -q "still reachable" "$logfile"; then
		echo " Warning: still reachable memory in $prog"
		return 1
	fi
	echo "No memory leaks or unsafe threads found in $prog."
	return 0
}

failed=0

cmake -S . -B build
cmake --build build

tmpfile=$(mktemp)
find ./build/bin/tests -type f -executable >"$tmpfile"

while read -r prog; do
	if ! check_program "$prog"; then
		failed=1
	fi
done <"$tmpfile"
rm -f "$tmpfile"

rm -rf build
rm -rf "$LOG_DIR"

if [ "$failed" -eq 1 ]; then
	echo "Some programs have memory leaks or unsafe threads!"
	exit 1
else
	echo "All checks passed! No memory leaks or unsafe threads detected."
	exit 0
fi
