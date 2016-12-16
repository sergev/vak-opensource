<?php
//
// Model-View-Controller implementation according to POSA
// (Pattern-Oriented Software Architecture
//   http://www.hillside.net/patterns/books/Siemens/book.html)
//

class HelloWorldController {
    private $model;
    function __construct($model) {
        $this->model = $model;
    }

    function handleEvent($args) {
        if (count($args) > 2) {
            $this->model->setStrategy($args[2]);
        }
        $this->model->addText(count($args) > 1 ? $args[1] : "");
    }
}

class HelloWorldModel {
    private $text;
    private $observers = array();
    private $strategy;

    function attach($observer) {
        $this->observers[] = $observer;
    }

    function getData() {
        $facade = new HelloWorldFacade($this->strategy);
        return $facade->getHelloWorld().$this->text."\n";
    }

    function addText($text='') {
        $this->text = $text;
        $this->notify();
    }

    function setStrategy($strategy) {
        $this->strategy = $strategy;
    }

    function notify() {
        foreach ($this->observers as $observer) {
            $observer->update();
        }
    }
}

class HelloWorldView {
    private $model;

    function initialize($model) {
        $this->model = $model;
        $model->attach($this);
        return $this->makeController();
    }

    function makeController() {
        return new HelloWorldController($this->model);
    }

    function update() {
        $this->display();
    }

    function display() {
        echo $this->model->getData();
    }
}

//
// "Business logic"
//

class HelloWorld {
   function execute() {
       return "Hello world";
   }
}

class HelloWorldDecorator {
   private $helloworld;
   function __construct($helloworld) {
       $this->helloworld = $helloworld;
   }

   function execute() {
       return $this->helloworld->execute();
   }
}

abstract class HelloWorldEmphasisStrategy {
    abstract function emphasize($string);
}

class HelloWorldBangEmphasisStrategy extends HelloWorldEmphasisStrategy {
    function emphasize($string) {
       return $string."!";
    }
}

class HelloWorldRepetitionEmphasisStrategy extends HelloWorldEmphasisStrategy {
    function emphasize($string) {
       return $string." and ".$string." again";
    }
}

class HelloWorldEmphasizer extends HelloWorldDecorator {
   private $strategy;
   function HelloWorldEmphasizer($helloworld,$strategy) {
       $this->strategy = $strategy;
       parent::__construct($helloworld);
   }

   function execute() {
       $string = parent::execute();
       return $this->strategy->emphasize($string);
   }
}

class HelloWorldStrategyFactory {
    static function make($type) {
        if ($type == 'repetition') return self::makeRepetitionStrategy();
        return self::makeBangStrategy();
    }

    static function makeBangStrategy() {
        return new HelloWorldBangEmphasisStrategy;
    }
    static function makeRepetitionStrategy() {
        return new HelloWorldRepetitionEmphasisStrategy;
    }
}

class HelloWorldFormatter extends HelloWorldDecorator {
   function execute() {
       $string = parent::execute();
       return $string."\n";
   }
}

class HelloWorldFacade {
    private $strategy;
    function __construct($strategyType) {
        $this->strategy = HelloWorldStrategyFactory::make($strategyType);
    }

    function getHelloWorld() {
        $formatter = new HelloWorldFormatter(
                new HelloWorldEmphasizer(
                    new HelloWorld,$this->strategy));
        return $formatter->execute();
    }
}

$model = new HelloWorldModel;
$view = new HelloWorldView;
$controller = $view->initialize($model);
$controller->handleEvent($_SERVER['argv']);

?>
