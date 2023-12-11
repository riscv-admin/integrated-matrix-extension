# Use the riscvintl/riscv-docs-base-container-image as a base image
FROM riscvintl/riscv-docs-base-container-image:latest

# Metadata as a label
LABEL maintainer="Rafael Sene rafael@riscv.org"

# Set the working directory in the container to /build
WORKDIR /build

# Run asciidoctor-pdf when the container launches
CMD sh -c "asciidoctor-pdf -o charter-$(date +%Y%m%d).pdf \
-a compress ./charter.adoc"
