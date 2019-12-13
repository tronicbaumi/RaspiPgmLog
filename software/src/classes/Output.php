<?php
/**
 * Class to handle output to browser
 */
Class Output{
    // store all data to output
    private $out = "";
    // buffer to data from view files
    private $data = "";
    
    public function __construct() {
    }
    
    /**
     * Read template content into class attribute
     * 
     * @param string $template path/to/template
     */
    public function LoadTemplate($template) {
        $this->out = file_get_contents($template);
    }
    
    /**
     * Add html string to $data attribute
     * 
     * @param string $string html string to add to output
     */
    public function Add($string) {
        $this->data .= $string;
    }
    
    /**
     * Insert html $data into $out and echo it to the browser 
     */
    public function out() {
        $this->out = str_replace("<!--###content###-->", $this->data, $this->out);
        echo $this->out;
    }
    
    /**
     * Provide one instance of output, to make sure there is always just one Output object filled with data
     * 
     * @staticvar Output $instance
     * @return \Output
     */
    public static function GetInstance() {
        static $instance = null;
        if($instance === null)
            $instance = new Output();
        
        return $instance;
    }
}

/**
 * Shortcut to Output instance
 * 
 * @return Output object
 */
function Out(){
    return Output::GetInstance();
}

