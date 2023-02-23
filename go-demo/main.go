package main

import (
	"fmt"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"strings"
)

var absdir string = ""

func getDirectoryListing(path string) ([]string, error) {
	f, err := os.Open(path)
	if err != nil {
		return nil, fmt.Errorf("error opening directory: %s", err)
	}

	// get the list of files
	files, err := f.Readdir(-1)
	if err != nil {
		return nil, fmt.Errorf("error reading directory: %s", err)
	}

	// write the list of files
	var flist []string
	for _, file := range files {
		flist = append(flist, "- "+file.Name())
	}

	return flist, nil
}

func getContentType(ext string) string {
	switch ext {
	case ".html":
		return "text/html"
	case ".css":
		return "text/css"
	case ".js":
		return "application/javascript"
	case ".png":
		return "image/png"
	case ".jpg":
		return "image/jpeg"
	case ".jpeg":
		return "image/jpeg"
	case ".gif":
		return "image/gif"
	case ".pdf":
		return "application/pdf"
	case ".svg":
		return "image/svg+xml"
	default:
		return "application/octet-stream"
	}
}

func handler(w http.ResponseWriter, r *http.Request) {
	// get the path from the request and remove the leading slash
	// in C, we'll be parsing the HTTP request ourselves
	path := r.URL.Path[1:]

	fpath := filepath.Join(absdir, path)
	fmt.Printf("User requested path \"%s\" which resolves to \"%s\"\n", path, fpath)

	fstat, err := os.Stat(fpath)
	if err != nil {
		w.WriteHeader(http.StatusNotFound)
		fmt.Fprintf(w, "404 Not Found")
		return
	} else if fstat.IsDir() {
		flist, err := getDirectoryListing(fpath)
		if err != nil {
			w.WriteHeader(http.StatusInternalServerError)
			fmt.Fprintf(w, "500 Internal Server Error")
			fmt.Printf("Error getting directory listing: %s", err.Error())
			return
		} else {
			fmt.Fprintf(w, "Directory listing for %s:\n\n%s", path, strings.Join(flist, "\n"))
			return
		}
	}

	// if it's not a directory, read the bytes
	// yes, I could use http.ServeFile, but I want to show the
	// expected functionality in C
	f, err := os.Open(fpath)
	if err != nil {
		w.WriteHeader(http.StatusInternalServerError)
		fmt.Fprintf(w, "500 Internal Server Error")
		fmt.Printf("Error opening file: %s", err.Error())
		return
	}

	// read the bytes
	buf := make([]byte, fstat.Size())
	_, err = f.Read(buf)
	if err != nil {
		w.WriteHeader(http.StatusInternalServerError)
		fmt.Fprintf(w, "500 Internal Server Error")
		fmt.Printf("Error reading file: %s", err.Error())
		return
	}

	// write the bytes
	w.WriteHeader(http.StatusOK)
	w.Header().Add("Content-Type", getContentType(filepath.Ext(fpath)[1:]))
	w.Write(buf)
}

func main() {
	// get directory from args
	dir := "."
	if len(os.Args) > 1 {
		dir = os.Args[1]
	} else {
		fmt.Println("No directory specified, defaulting to current directory")
	}

	// get absolute path
	var err error
	absdir, err = filepath.Abs(dir)
	if err != nil {
		log.Fatal(err)
	}

	// ensure it exists and is a directory
	fi, err := os.Stat(absdir)
	if err != nil {
		log.Fatalf("Error accessing %s, ensure it's a valid directory you can access", absdir)
	} else if !fi.IsDir() {
		log.Fatalf("%s is not a directory", absdir)
	}

	// we're going to do this all manually in C!
	http.HandleFunc("/", handler)

	fmt.Printf("Listening on port 8080 and serving from %s\n", absdir)
	log.Fatal(http.ListenAndServe(":8080", nil))
}
