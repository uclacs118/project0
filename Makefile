build:
	docker build -t project0 -f autograder/Dockerfile .

publish:
	docker tag project0 ghcr.io/uclacs118/project0
	docker push ghcr.io/uclacs118/project0
